var width = 1000, height = 550;
var radius = 10;
var xPos = width/2;
var yPos = height/2;
var xVel = 4;
var yVel = 0;
var paddle1Xpos = 25; 
var paddle1Ypos = 235;
var paddle2Xpos = 975;
var paddle2Ypos = 235;
var paddleLen = 80;
var paddleVel = 8;
var lineCap = ['round'];
var player1 = 0; //red paddle score
var player2 = 0; //blue paddle score
var endScore = 3; //score that ends game
var ctx;


function initial()
{
	ctx = document.getElementById('canvas').getContext('2d');
	return setInterval(animate, 10);
}

//***********************************************************************

function drawTemplate()
{
	var lineJoin = ['round'];
	
	// Change for no Shadowing 
	ctx.shadowOffsetX = 0;
	ctx.shadowOffsetY = 0;
	ctx.shadowBlur = 0;
	ctx.shadowColor = "rgba(0, 0, 0, 0)";
	
	// Draw board
	ctx.strokeStyle = 'black';
	ctx.lineJoin = lineJoin;
	ctx.lineWidth = 2;
	ctx.beginPath();
	ctx.moveTo(5, 5);
	ctx.lineTo(width, 5);
	ctx.lineTo(width, height);
	ctx.lineTo(5, height);
	ctx.closePath();
	ctx.moveTo(500, 5);
	ctx.lineTo(500, height);
	ctx.stroke();
}

//***********************************************************************

function drawBall(xPos, yPos, radius)
{	
	ctx.beginPath();
	
	ctx.fillStyle = 'rgb(50, 255, 50)'; //black

	ctx.arc(xPos, yPos, radius, 0, Math.PI*2, true);
	ctx.closePath();
	ctx.fill();
}

//***********************************************************************

function drawPaddle1(paddle1Xpos, paddle2Xpos)
{
	ctx.strokeStyle = 'red';
	ctx.lineCap = lineCap;
	ctx.lineWidth = 15;
	ctx.beginPath();
	ctx.moveTo(paddle1Xpos, paddle1Ypos);
	ctx.lineTo(paddle1Xpos, (paddle1Ypos+paddleLen));
	ctx.stroke();
}

//***********************************************************************

function drawPaddle2(paddle1Xpos, paddle2Xpos)
{
	ctx.strokeStyle = 'blue';
	ctx.lineCap = lineCap;
	ctx.lineWidth = 15;
	ctx.beginPath();
	ctx.moveTo(paddle2Xpos, paddle2Ypos);
	ctx.lineTo(paddle2Xpos, (paddle2Ypos+paddleLen));
	ctx.stroke();
}

//***********************************************************************

function clearCanvas() 
{
	ctx.clearRect(0, 0, width, height);
}

//***********************************************************************

function resetField(player1, player2)
{		
	xPos = width/2;
	yPos = height/2;
	paddle1Ypos = 235;
	paddle2Ypos = 235;
	
	if (player1 > player2)
		xVel = 4;
	else
		xVel = -4;
		
	yVel = 0;	
	animate();
}

//***********************************************************************

function animate()
{	
	// resets field when ball passes paddle
	if (xPos > width) {
		player1 += 1;
		resetField(player1, player2);
	}
	if (xPos < 0) {
		player2 += 1;
		resetField(player1, player2);
	}
	
	// Displays score above cavans
	document.getElementById('scores').innerHTML = "<pre><h2>Player 1: " + player1 + "			  					" + "Player 2: " + player2 + "</h2></pre>";
		
	if (player1 == endScore || player2 == endScore) {
		if (player1 > player2) {
			alert("Player 1 Wins!");
		}
		else {
			alert("Player 2 Wins!");
		}
		return; 
	}
	
	clearCanvas();
	drawTemplate();
	
	//Create shadowing for ball and paddles
	ctx.shadowOffsetX = 2;
	ctx.shadowOffsetY = 2;
	ctx.shadowBlur = 2;
	ctx.shadowColor = "rgba(0, 0, 0, 0.5)";
	
	drawBall(xPos, yPos, radius);
	drawPaddle1(paddle1Xpos, paddle1Ypos);
	drawPaddle2(paddle1Xpos, paddle2Xpos);
		  
	xPos += xVel;
	yPos += yVel;
	
	// Checks if ball makes contact with walls
	if (yPos > (height-radius))
		yVel = -yVel;
	if (yPos < radius+5)
		yVel = -yVel;
		
	// checks for user input
	document.onkeydown = function(event) {
		var inKey
		if (event == null) 
			inKey = window.event.keyCode;
		else 
			inKey = event.keyCode;
		
		switch (inKey) {
		case 87: // w key
			if (paddle1Ypos > 2*radius)
				paddle1Ypos -= paddleVel;
			break;
		case 83: // s key
			if ((paddle1Ypos+paddleLen) < (height-2*radius))
				paddle1Ypos += paddleVel;
			break;	
		case 79: // o key
			if (paddle2Ypos > 2*radius)
				paddle2Ypos -= paddleVel;
			break;
		case 75: // k key
			if ((paddle2Ypos+paddleLen) < (height-2*radius))
				paddle2Ypos += paddleVel;
			break;
		}
	} // End function(event)
	
	// checks for ball to paddle contact
	// determines direction of ball based on contact position w/ paddle
	if (xPos < (paddle1Xpos+radius+7.5)) {
		if (yPos > paddle1Ypos-5 && yPos < (paddle1Ypos+paddleLen+5)) {
			xVel = -xVel;
			if (yPos < (paddle1Ypos+(paddleLen/2)-paddleVel)) {
				yVel = -Math.floor(Math.random()*4);
			}
			else if (yPos > (paddle1Ypos+(paddleLen/2)+paddleVel)) {
				yVel = Math.floor(Math.random()*4);
			}
			else {
				yVel = 0;
			}
		}
	}
		
	if (xPos > (paddle2Xpos-radius-7.5)) {
		if (yPos > paddle2Ypos-5 && yPos < (paddle2Ypos+paddleLen+5)) {
			xVel = -xVel;
			if (yPos < (paddle2Ypos+(paddleLen/2)-paddleVel)) {
				yVel = -Math.floor(Math.random()*4);
			}
			else if (yPos > (paddle2Ypos+(paddleLen/2)+paddleVel)) {
				yVel = Math.floor(Math.random()*4);
			}
			else {
				yVel = 0;
			}
		}
	} 
		
} // End function animate