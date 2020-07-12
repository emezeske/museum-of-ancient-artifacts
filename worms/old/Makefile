JC = javac
GAME_CLASSES = 	Camera.java \
		Worm.java \
		FreeRectBody.java \
		Terrain.java \
		WormGame.java \
		Bazooka.java \
		Weapon.java \
		Grenade.java \
		Explosion.java \
		Firework.java \
		Confetti.java \
		SmokePuff.java \
		Flame.java

.SUFFIXES: .java .class
.java.class:
	$(JC) $*.java

all: classes

classes: $(GAME_CLASSES:.java=.class)
	cp *.class /pub_html/wormgame

clean:
	rm *.class
