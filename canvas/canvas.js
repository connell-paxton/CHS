var canvas = document.getElementById("game");

var ctx = canvas.getContext('2d');

ctx.fillStyle = 'rgba(0,0,0,0.4)';
ctx.strokeStyle = 'rgba(0,153,255,0.4)';
ctx.save();
ctx.translate(150,150);
var time = new Date();
ctx.rotate( ((2*Math.PI)/6)*time.getSeconds() + ( (2*Math.PI)/6000)*time.getMilliseconds() );
ctx.translate(0,28.5);
ctx.drawImage(pattern,-3.5,-3.5);
ctx.restore();

