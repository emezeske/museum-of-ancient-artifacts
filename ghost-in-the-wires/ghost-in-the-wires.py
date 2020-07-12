#!/bin/python

from itertools import cycle
from itertools import chain
from pprint import pprint
import textwrap
import random
from collections import defaultdict
from multiprocessing import Pool
import math

with open('/home/emezeske/ghost-in-the-wires.txt', 'r') as fh:
  REPRESENTATIVE_TEXT = ''.join([c for c in fh.read().lower() if 'a' <= c <= 'z' or c == ' '])
  REPRESENTATIVE_WORDS = REPRESENTATIVE_TEXT.split()

WORD_DISTRIBUTION = defaultdict(int)
WORD_TRANSITIONS = defaultdict(lambda: defaultdict(int))

for a, b in zip(REPRESENTATIVE_WORDS, REPRESENTATIVE_WORDS[1:]):
  WORD_DISTRIBUTION[a] += 1
  WORD_TRANSITIONS[a][b] += 1

def LogNormalizeDistribution(distribution):
  log_total = math.log(sum(distribution.values()))
  for key, value in distribution.iteritems():
    distribution[key] = math.log(value) - log_total

LogNormalizeDistribution(WORD_DISTRIBUTION)
for word in WORD_TRANSITIONS:
  LogNormalizeDistribution(WORD_TRANSITIONS[word])

DICTIONARY_WORDS = set()

with open('/usr/share/dict/words', 'r') as fh:
  for line in fh:
    DICTIONARY_WORDS.add(''.join([c for c in line.strip().lower() if 'a' <= c <= 'z']))

ALL_WORDS = set(REPRESENTATIVE_WORDS) | DICTIONARY_WORDS

def AlphaIndex(c):
  return ord(c) - ord('a')

def Shift(c, by):
  return chr(ord('a') + (AlphaIndex(c) + by) % 26)

def Multiply(c, by):
  return chr(ord('a') + (AlphaIndex(c) * by) % 26)

def Xor(c, by):
  return chr(ord('a') + (AlphaIndex(c) ^ by) % 26)

def EnumerateNgraphs(n, base='', depth=0):
  if depth == n:
    return [base]
  ngraphs = []
  for i in range(26):
    ngraphs.extend(EnumerateNgraphs(n, base + chr(ord('a') + i), depth + 1))
  return ngraphs

def NgraphDistribution(source, n=1):
  distribution = {}
  for ngraph in EnumerateNgraphs(n):
    distribution[ngraph] = 0
  next_ngraph = ''
  for c in source:
    if 'a' <= c <= 'z':
      next_ngraph += c
      if len(next_ngraph) == n:
        distribution[next_ngraph] += 1
        next_ngraph = next_ngraph[1:]
  total = sum(distribution.values())
  for ngraph in distribution.keys():
    distribution[ngraph] /= float(total)
  return distribution

def TopFromDistribution(distribution, top_n):
  return sorted(distribution.items(), key=lambda x: x[1], reverse=True)[:top_n]

UNIGRAPH_DISTRIBUTION = NgraphDistribution(REPRESENTATIVE_TEXT)
DIGRAPH_DISTRIBUTION = NgraphDistribution(REPRESENTATIVE_TEXT, n=2)
TRIGRAPH_DISTRIBUTION = NgraphDistribution(REPRESENTATIVE_TEXT, n=3)

def ChiSquared(reference, candidate):
  chi_squared = 0.0
  for k in reference.keys():
    chi_squared += ((candidate[k] - reference[k]) ** 2) / reference[k]
  return chi_squared

def CountWords(s):
  min_word_len = 4
  longest_word = ''
  word_count = 0
  for i in range(len(s) - min_word_len):
    for j in range(min_word_len, len(s) - i):
      w = s[i:i+j]
      if w in ALL_WORDS:
        word_count += 1
        if len(w) > len(longest_word):
          longest_word = w
  return longest_word, word_count

def RankCandidatePlaintexts(candidates, filtered=True):
  filtered_with_stats = []
  for (pt, info) in candidates:
    pt_str = ''.join(pt)
    min_word_len = 4
    longest_word, word_count = CountWords(pt_str) 
    if not filtered or word_count >= 5:
      chi_squared = ChiSquared(UNIGRAPH_DISTRIBUTION, NgraphDistribution(pt))
      filtered_with_stats.append((pt_str, info, chi_squared, longest_word, word_count))

  best = []
  if filtered_with_stats:
    print 'Top plaintext candidates:'
    for (pt, info, chi_squared, longest_word, word_count) in sorted(filtered_with_stats, key=lambda x: x[2])[:10]:
      print '%s (info=%s, chi^2=%s, longest=%s, words=%s)' % (pt, info, chi_squared, longest_word, word_count)
      best.append(pt)
  else:
    print 'No good candidates.'
  return best

LONGEST_OF_ALL_WORDS = max([len(word) for word in ALL_WORDS])

def MatchWords(s, matched=None):
  if matched is None:
    matched = ()
  results = []
  if not s:
    if matched:
      return [matched]
    else:
      return []
  for i in range(1, min(LONGEST_OF_ALL_WORDS, len(s)) + 1):
    word = s[:i]
    # Could try to handle suffixes, etc here
    if (len(word) == 1 and word in ('i', 'a')) or (len(word) > 1 and word in ALL_WORDS):
      results.extend(MatchWords(s[i:], matched + (word,)))
  return results

LEAST_SEEN_LIKELIHOOD = sorted(WORD_DISTRIBUTION.iteritems(), key=lambda x: x[1])[0][1]

def MostLikelySentence(sentences):
  best_average_likelihood = float('-inf')
  best_sentence = None
  for sentence in sentences:
    if len(sentence) == 0:
      continue
    likelihood = 0.0
    for word in sentence:
      if word in WORD_DISTRIBUTION:
        likelihood += WORD_DISTRIBUTION[word]
      else:
        # If we didn't see this word in the frequency corpus, assume it is
        # as likely as the least likely word we did see.
        likelihood += LEAST_SEEN_LIKELIHOOD
    # This is not a perfect metric, but is decent. Some kind of per-word
    # normalization is needed to prevent a single word sentence like "the"
    # from dominating.
    average_likelihood  = likelihood / len(sentence)
    if average_likelihood > best_average_likelihood:
      best_average_likelihood = average_likelihood
      best_sentence = sentence
  return best_sentence

def FreqAnalysis(ct, breadth=6, depth=6):
  def TopMatch(ref, emp, translate):
    if not ref:
      return [dict(translate)]
    translates = []
    fro, _ = emp[0]
    for i in range(min(breadth, len(ref))):
      new_ref = list(ref)
      to, _ = new_ref.pop(i)
      translates.extend(TopMatch(new_ref, emp[1:], list(translate) + [(fro, to)]))
    return translates

  ref = TopFromDistribution(UNIGRAPH_DISTRIBUTION, depth)
  pprint(ref)
  emp = TopFromDistribution(NgraphDistribution(ct), depth)
  pprint(emp)

  translates = TopMatch(ref, emp, [])
  pts = []
  for translate in translates:
    pts.append(([translate.get(c, '.') for c in ct], translate))
  print 'Freq analysis?'
  RankCandidatePlaintexts(pts, filtered=False)


print 'Chapter 1 ' + '-' * 80
ct = 'Max vhlm hy max unl wkboxk ingva B nlxw mh ingva fr hpg mktglyxkl'
# noticed that "B" was 7 below "I", rotate by 7
print ''.join(map(lambda x: Shift(x, 7) if x != ' ' else x, ct.lower()))
# answer: 15 dollars

print 'Chapter 2 ' + '-' * 80
ct = 'Estd mzzv esle elfrse xp szh ez ncplep yph topyetetpd hspy T hld l acp-eppy'
# used rotation of 15 from answer to previous puzzle
print ''.join(map(lambda x: Shift(x, 15) if x not in " -" else x, ct.lower()))
# answer: The Paper Trip

print 'Chapter 3 ' + '-' * 80
ct = 'pbzfsobp dkfobtpkx lq pbkfi ppbkfpry aoxtolc iixz lq abpr bobt pbzfsba cl bmvq obail bpbeQ'
key = 'The Paper Trip'

#FreqAnalysis(ct.lower()[::-1].translate(None, ' '))

# found through brute forcing rotations, and guessing reverse due to the capital Q
# not sure how the key is involved? maybe 3 words?
pts = []
for i in range(26):
  pts.append(([Shift(c, i) if c != ' ' else ' ' for c in ct.lower()[::-1]], i))

  # Slow but very effective:
  #sentences = MatchWords(''.join([Shift(c, i) if c != ' ' else '' for c in ct.lower()[::-1]]))
  #if sentences:
  #  sentence = MostLikelySentence(sentences)
  #  if sentence is not None:
  #    print 'Num possible sentences:', len(sentences)
  #    print 'Best sentence:', sentence
RankCandidatePlaintexts(pts)
# answer: diverters

print 'Chapter 4 ' + '-' * 80

# NOTE gsv is repeated several times
# rm appears twice
# zn appears twice
# nv

# diverter
# gsvmznvl
# ugsvnzrm
# uiznvhrs
# zxpvwzgf
# hxrmgsvz
# ikzmvgwz
# bh

# diverters
# gsvmznvlu
# gsvnzrmui
# znvhrszxp
# vwzgfhxrm
# gsvzikzmv
# gwzbh

# gsvmznvlug
# svnzrmuizn
# vhrszxpvwz
# gfhxrmgsvz
# ikzmvgwzbh

# gsvmz
# nvlug
# svnzr
# muizn
# vhrsz
# xpvwz
# gfhxr
# mgsvz
# ikzmv
# gwzbh

# no z
# gsvm nvlugsvn rmui nvhrs xpvw gfhxrmgsv ik mvgw bh

# no v
# gs mzn lugs nzrmuizn hrszxp wzgfhxrmgs zikzm gwzbh

# gsvmznvlugsvnzrmuiznvhrszxpvwzgfhxrmgsvzikzmvgwzbh

ct = 'gsvmznvlugsvnzrmuiznvhrszxpvwzgfhxrmgsvzikzmvgwzbh'

ref = map(lambda (x, y): x, TopFromDistribution(UNIGRAPH_DISTRIBUTION, 26))
emp = map(lambda (x, y): x, TopFromDistribution(NgraphDistribution(ct), 26))
print ref
print emp

translate = dict(zip(emp, ref))
print ''.join([translate[c] for c in ct])

#for i in range(26):
#  print ''.join([Shift(c, -AlphaIndex(k)) for i, k in zip(ct, ct[::-1])])

pairs = zip('mkq', 'mkq')
print ''.join([Shift(c, AlphaIndex(k)) for c, k in pairs])

pairs = zip('gsvmznvlu', 'gsvnzrmui')
print ''.join([Shift(c, AlphaIndex(k)) for c, k in pairs])

ct = 'gsvmznvlugsvnzrmuiznvhrszxpvwzgfhxrmgsvzikzmvgwzbh'
pairs = zip(ct, cycle('yug'))
print ''.join([Shift(c, -AlphaIndex(k)) for c, k in pairs])

# Try to treat each column as a caesar cipher
#columns = ['ggzvgg', 'ssnwsw', 'vvvzvz', 'mnhgzb', 'zzrfih', 'nrshk', 'vmzxz', 'luxrm', 'uipmv']
#guesses = []
#for column in columns:
#  pts = []
#  for i in range(26):
#    pts.append(([Shift(c, i) for c in column], i))
#  print 'Caesar Cipher for colunn:', column
#  best = RankCandidatePlaintexts(pts, False)[:3]
#  guesses.append(best)
#
#def _ConcatGuesses(guesses):
#  first = guesses.pop(0)
#  for i in len(guesses);

ct = 'gsvmznvlugsvnzrmuiznvhrszxpvwzgfhxrmgsvzikzmvgwzbh'
key = 'diverters'

# maybe try divertsabcfghjklmnopquwxyz replacement cipher

print 'Replacement cipher?'
replacement = dict(zip('divertsabcfghjklmnopquwxyz', 'abcdefghijklmnopqrstuvwxyz'))
print ''.join([replacement[c] for c in ct])

replacement = dict(zip('abcdefghijklmnopqrstuvwxyz', 'abcdefghijklmnopqrstuvwxyz'[::-1]))
pprint(replacement)
print ''.join([replacement[c] for c in ct])

#replacement = dict(zip('gsvzm', 'thaen'))
#print ''.join([replacement.get(c, c.upper()) for c in ct])

pprint(TopFromDistribution(UNIGRAPH_DISTRIBUTION, 26))
pprint(TopFromDistribution(NgraphDistribution(ct, 1), 26))

pprint(TopFromDistribution(DIGRAPH_DISTRIBUTION, 10))
pprint(TopFromDistribution(NgraphDistribution(ct, 2), 10))

pprint(TopFromDistribution(TRIGRAPH_DISTRIBUTION, 10))
pprint(TopFromDistribution(NgraphDistribution(ct, 3), 10))

# TODO look more "top N most likely words"?

# Hill climbing on substitutions does not work this way.
#alphabet = 'abcdefghijklmnopqrstuvwxyz'
#replacement = dict(zip(alphabet, alphabet))
#all_time_best_chi_sq = float('inf')
#all_time_best_word_count = 0
#all_time_best_longest_word = 0
#all_time_best_pt = ''
#all_time_best_score = 0.0
#current_best_chi_sq = float('inf')
#current_best_word_count = 0
#current_best_longest_word = 0
#current_best_pt = ''
#current_best_score = 0.0
#
##for _ in xrange(1000000):
#while True:
#  old_replacement = dict(replacement)
#  for _ in range(random.randint(1, len(alphabet) + 1)):
#    a, b = random.choice(alphabet), random.choice(alphabet)
#    replacement[a], replacement[b] = replacement[b], replacement[a]
#  pt = [replacement[c] for c in ct]
#  pt_str = ''.join(pt)
#  chi_sq = ChiSquared(UNIGRAPH_DISTRIBUTION, NgraphDistribution(pt))
#  longest_word, word_count = CountWords(pt_str) 
#  score = (len(longest_word) + word_count / 2.0) / (0.00000001 + chi_sq * 10.0)
#  if ((chi_sq <= all_time_best_chi_sq
#       and word_count >= all_time_best_word_count
#       and len(longest_word) >= all_time_best_longest_word)
#      or score > all_time_best_score):
#    all_time_best_chi_sq = chi_sq
#    all_time_best_word_count = word_count
#    all_time_best_longest_word = len(longest_word)
#    all_time_best_score = score
#    all_time_best_pt = pt_str
#    pprint(replacement)
#
#    current_best_chi_sq = chi_sq
#    current_best_word_count = word_count
#    current_best_longest_word = len(longest_word)
#    current_best_score = score
#    current_best_pt = pt_str
#    print current_best_chi_sq
#    print current_best_word_count
#    print current_best_longest_word
#    print longest_word
#    print current_best_pt
#  elif ((chi_sq <= current_best_chi_sq
#         and word_count >= current_best_word_count
#         and len(longest_word) >= current_best_longest_word)
#        or score > current_best_score
#        or random.random() < 0.01):
#    current_best_chi_sq = chi_sq
#    current_best_word_count = word_count
#    current_best_longest_word = len(longest_word)
#    current_best_score = score
#    current_best_pt = pt_str
#  else:
#    replacement = old_replacement

#FreqAnalysis(ct)


#sentences = MatchWords('this book that taught me how to create new identities when i was a pre-teen'.translate(None, ' -'))
#pprint(sentences)
#pprint(MostLikelySentence(sentences))
#import sys
#sys.exit(0)

# For testing
#pairs = zip('thisbookthatttaughtmehowtocreate', cycle('walrus'))
#ct = ''.join([Shift(c, AlphaIndex(k)) for c, k in pairs])

############### these work on test ciphers.... but not the real one

def PolyalphabeticBruteForce((ciphertext, sign, caesar_offset)):
  print 'Trying:', ciphertext, sign, caesar_offset
  pts = []
  for word in ALL_WORDS:
    pairs = zip(ciphertext, cycle(word))
    pt = [Shift(c, (sign * AlphaIndex(k) + caesar_offset)) for c, k in pairs]
    pt_str = ''.join(pt)
    sentences = MatchWords(pt_str)
    if sentences:
      sentence = MostLikelySentence(sentences)
      if sentence is not None:
        pts.append((pt, (sign, caesar_offset, word)))
  return pts

print 'Brute forced Polyalphabetic cipher?'
pool = Pool(4)
args = []
for ciphertext in (ct, ct[::-1]):
  for sign in (-1, 1):
    for caesar_offset in range(26):
      args.append((ciphertext, sign, caesar_offset))
pts = chain(*pool.map(PolyalphabeticBruteForce, args))
RankCandidatePlaintexts(pts, filtered=False)

def AutokeyCipherBruteForce((ciphertext, sign, caesar_offset)):
  print 'Trying:', ciphertext, sign, caesar_offset
  pts = []
  for word in ALL_WORDS:
    fullkey = word
    pt = []
    for i, c in enumerate(ciphertext):
      n = Shift(c, sign * AlphaIndex(fullkey[i]) + caesar_offset)
      pt.append(n)
      fullkey += n
    pt_str = ''.join(pt)
    sentences = MatchWords(pt_str)
    if sentences:
      sentence = MostLikelySentence(sentences)
      if sentence is not None:
        pts.append((pt, (sign, caesar_offset, word)))
  return pts

print 'Brute forced Autokey cipher?'
pool = Pool(4)
args = []
for ciphertext in (ct, ct[::-1]):
  for sign in (-1, 1):
    for caesar_offset in range(26):
      args.append((ciphertext, sign, caesar_offset))
pts = chain(*pool.map(AutokeyCipherBruteForce, args))
RankCandidatePlaintexts(pts, filtered=False)

print 'Caesar Cipher?'
pts = []
for i in range(26):
  pts.append(([Shift(c, i) for c in ct], i))
RankCandidatePlaintexts(pts)

print 'Polyalphabetic with shifted key?'
pts = []
for i in range(len(key)):
  pairs = zip(ct, cycle(key[i:] + key[:i]))
  pts.append(([Shift(c, AlphaIndex(k)) for c, k in pairs], i))
RankCandidatePlaintexts(pts)

print 'Polyalphabetic+Caesar or XOR?'
pairs = zip(ct, cycle(key))
pts = []
for i in range(26):
  # Does not appear to be a simple Polyalphabetic Cipher, even with rotation...
  pts.append(([Shift(c, (AlphaIndex(k) + i) % 26) for c, k in pairs], 'pos + %s' % i))
  pts.append(([Shift(c, -((AlphaIndex(k) + i) % 26)) for c, k in pairs], 'neg + %s' % i))
  # Does not appear to be a simple XOR Cipher...
  pts.append(([Xor(c, ((AlphaIndex(k) + i) % 26)) for c, k in pairs], 'xor + %s'))
RankCandidatePlaintexts(pts)

print 'Polyalphabetic with shifted key?'
pts = []
for i in range(len(key)):
  pairs = zip(ct, cycle(key[i:] + key[:i]))
  pts.append(([Shift(c, AlphaIndex(k)) for c, k in pairs], i))
RankCandidatePlaintexts(pts)

print 'Multiplicative with shifted key?'
pts = []
for i in range(len(key)):
  pairs = zip(ct, cycle(key[i:] + key[:i]))
  pts.append(([Multiply(c, AlphaIndex(k)) for c, k in pairs], i))
RankCandidatePlaintexts(pts)

print 'Chapter 5 ' + '-' * 80

# maybe a 3d ADFGVX cipher? https://en.wikipedia.org/wiki/ADFGVX_cipher
ct = 'jbi ujt veo eco ntk iwa lhc eeo anu uir trs hae oni rfn irt toh imi ets shs !eu'
