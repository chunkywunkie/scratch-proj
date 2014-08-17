from graphics import *
import math, random, sys, time

# Global variables
TITLE = "Bug Buster"
WIDTH, HEIGHT = 600,600
WIN_DIM = ((0,0),(6,6))
GRID_DIM = (4,4)
SPRAY_POS = ((3.5,.25),(5.5,.75))
TRIES_POS = (1,0.5)
INSTRUCT_POS = (3,5.25)
RESULT_POS = (3,5.5)
MAX_TRIES = 5

def setup_Grid( w, h):
	""" Creates and collects rectangles to represent grid
	w - integer representing number of rectangles in width
	h - integer representing number of rectangles in height
	Returns: list of rectangles to represent grid """
	
	grid = []
	# Have grid generate left-right, top-down
	h,w = range(h), range(w)
	for y in h:
		for x in w:
			rect = Rectangle(Point(x+1,y+1), Point(x+2,y+2))
			rect.setOutline('black')
			grid.append(rect)
	return grid

def setup_SprayButton(tl,br):
	sprayButton = Rectangle(Point(tl[0],tl[1]),Point(br[0],br[1]))
        sprayButton.setFill("pink")
        sprayButton.setOutline("black")	
	sprayText = setup_text((tl[0]+1,tl[1]+0.25), "Spray everywhere!!")
	return [sprayButton, sprayText]

def setup_text(pos, msg=""):
	return Text(Point(pos[0],pos[1]),msg)

def setup_image(x,y, src):
	img = Image(Point(x,y), src)
	return img

def initialBugPos():
	bx,by = random.randint(1,4),random.randint(1,4)
	return bx,by

def drawImage(img,win):
	img.move(0.5,0.5) # center image on tile
	img.draw(win)

# BEGIN Given methods
def move(x,y):
	while 1:
		rx = random.randint(-1,1)
		ry = random.randint(-1,1)
		
		# Check if bug moves out of grid
		if x+rx < 1 or x+rx > 4 or y+ry > 4 or y+ry < 1:
			continue
		else:
			return rx,ry

def showClick(win):
	pClick = win.getMouse()
	intX = int(pClick.x)
	intY = int(pClick.y)
	clickCir = Circle(Point(intX+0.5, intY+0.5), 0.4)
	clickCir.setFill("pink")
	clickCir.draw(win)
	time.sleep(0.5)
	clickCir.undraw()
	return Point(intX, intY)
# END Given Methods

def differentMove(bx,by,bugVisited):
	"""
	For the bug trace to work, we cannot repeat past visits
	bx: current bug's x position
	by: current bug's y position
	bugVisited: positions bugs already visited
	Return: True if this is a new position visited
	"""
	for visits in bugVisited:
		if visits[0] == bx and visits[1] == by:
			return False
	return True

def bugTrace(pos,win):
	traces = [setup_text(pos[i],str(i+1)) for i in xrange(len(pos))]
	for t in traces: 
		t.setSize(30); drawImage(t,win)
	

def moveBug(bx,by,bugPos):
	dx,dy = 0,0
	# We want the bug to actually move
	while dx == 0 and dy == 0:
		dx,dy = move(bx,by)
		# temp variables
		tx = bx+dx; ty = by+dy # temp variables
		if not differentMove(tx,ty,bugPos):
			dx = 0; dy = 0
	bx+= dx; by+=dy # hidden bug position
	return bx,by,dx,dy

def main():
	try:
		win = GraphWin(TITLE, WIDTH, HEIGHT)
		win.setCoords(WIN_DIM[0][0], WIN_DIM[0][1], 
		              WIN_DIM[1][0], WIN_DIM[1][1])

		# Setup ui elements
		grid = setup_Grid(GRID_DIM[0], GRID_DIM[1])
		sprayButton = setup_SprayButton(SPRAY_POS[0], SPRAY_POS[1])
		instructions = setup_text(INSTRUCT_POS, "Click a square to catch the bug")
		numTriesLeft = setup_text(TRIES_POS, "Number of tries left: " + str(MAX_TRIES))	
		gameResult = setup_text(RESULT_POS)
		
		# Set up initial bug
		bx,by = initialBugPos()
		bugHint = setup_image(bx,by,"bug.gif")
		
		# Draw ui elements
		for r in grid: r.draw(win)
		instructions.draw(win)
		numTriesLeft.draw(win)
		gameResult.draw(win) # Note that this is empty
		drawImage(bugHint,win)
		
		# Game variables
		tries = MAX_TRIES
		done, sprayUsed = False, False
		redo = False 
		bugPos = []
		
		# Game loop
		while not done and tries > 0:
			instructions.setText("Click a square to catch the bug")
			showSpray = tries <= 2 
			if tries == 2 and not redo: 
				for e in sprayButton: e.draw(win)

			userClick = showClick(win)
			# Check if valid user click
			inGrid = userClick.x >= 1 and userClick.x <= 4 and \
				    userClick.y >= 1 and userClick.y <= 4
			clickSpray = userClick.x >= math.floor(SPRAY_POS[0][0])\
					and userClick.y >= math.floor(SPRAY_POS[0][1]) and \
					userClick.x <= math.floor(SPRAY_POS[1][0]) and \
					userClick.y <= math.floor(SPRAY_POS[1][1])
		
			onBug = bx == userClick.x and by == userClick.y	
			nextToBug = onBug == False and bx-userClick.x >= -1 and \
				 bx-userClick.x <= 1 and by-userClick.y >= -1 and \
				 by-userClick.y <= 1

			if inGrid:
				if nextToBug:
					bx,by,dx,dy = moveBug(bx,by,bugPos)
					if userClick.x == bx and userClick.y == by:
						done = True
					else:
						bugHint.move(dx,dy)
						bugPos.append((bx,by))	
				else:
					if onBug:
						instructions.setText("Do not click bug")
					else:
						instructions.setText("Please click next to the bug")
					time.sleep(1)
					redo = True
					continue
			elif showSpray and clickSpray:
				sprayButton[0].setFill('red')
				time.sleep(0.5)
				sprayButton[0].setFill('pink')
				bx,by,dx,dy = moveBug(bx,by,bugPos)
				done = True; sprayUsed = True
				continue
			else:
				instructions.setText('Please click inside the grid')
				time.sleep(1)
				redo = True
				continue	
			tries-=1
			redo = False
			numTriesLeft.setText("Number of tries left: " + str(tries))

		# Win and lose messages
		bugHint.undraw()
		if done and not tries == 0:
			bugKilled = setup_image(bx,by, "bug_die.gif")
			drawImage(bugKilled,win)
			if sprayUsed:
				gameResult.setText("You used spray to kill the bug")
				
			else:
				gameResult.setText("Congratulations! Bug is now killed")
		else:
			bugWin = setup_image(bx,by, "bug_win.gif")
			drawImage(bugWin, win)
			gameResult.setText("Bug survives!!")
			bugTrace(bugPos,win)
			
		# Click anywhere to exit
		instructions.setText('Click anywhere to exit')	
		win.getMouse()
		win.close()

	except GraphicsError as e:
		print e
		sys.exit(1)
main()





