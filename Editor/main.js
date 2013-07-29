Function.prototype.method = function (name, func) {
	this.prototype[name] = func;
	return this;
};

Array.method('indexMin', function(compareFunc){
	var minIndex = 0
		, i;
		
	compareFunc = compareFunc || sortfunc;

	for(i = 1; i < this.length; i += 1){
		if(compareFunc(this[minIndex], this[i]) > 0){
			minIndex = i;
		}
	}
	
	return minIndex;
});

Array.method('min', function(compareFunc){
	return this[this.indexMin(compareFunc)];
});

Array.method('extract', function(i){
	var elem = this[i];
	this.splice(i, 1);
	return elem;
});

Array.method('extractMin', function(compareFunc){
	return this.extract(this.indexMin(compareFunc));
});

// points = [{x:0, y:0}, {x:0, y:5}, {x: 10, y: 10}, {x: 1, y: 5}];
// console.log(convexHull(points));

//TODO:
/*
- two ways of drawing objects - dotted border or filled (aspect)

- some new objects? I have many more ideas, these are only very basic ones, so I will be able to finish on time xD
*/

// I know the number of globals is outrageous, but I'm in a hurry

const pi = Math.PI,
			dwapi = pi*2,
			pi32 = pi*1.5,
			pipol = pi/2,
			pi4 = pi/4,
			pi34 = pi4*3,
			stopni15 = pi/12,
			jedenstopien = pi/180,
			polstopnia = pi/360,
			jedna50stopnia = pi/3600,
			sqrt2 = Math.sqrt(2),
			sqrt2przez2 = sqrt2/2;

var $disp,$canva,$ctx,$ImgD,$a,$b,$stroke,
	 $BackgroundText,
	 $ObjListSelect,
	 $ObjListParent,
	 $OptGroups = {
		 $OptPlayers:0,
		 $OptCircles:0,
		 $OptRectangles:0,
		 $OptSquares:0,
		 $OptEntrances:0,
		 $OptExits:0,
		 $OptSwitches:0,
		 $OptDoors:0,
		 $OptParticles:0
	 },
	 $firstP,
	 $secondP,
	 $thirdP,
	 $forthP,
	 $fifthP,
	 $sixthP,
	 $seventhP,
	 $angleP,
	 $r0P,
	 $colorP,
	 $SpanObjType,
	 $sieczka;
const wdth=750,
		hght=750,
		SelectionColor='rgba(96, 96, 255, 0.85)',
		selectRectColor = 'rgba(70, 80, 255, 0.3)',
		copyColor = 'rgba(0, 255, 0, 0.9)',
		PlayerSize = 15,
		MaxCharge = 1000,
		MaxPos = 100;

var	SQR = square({x:449,y:449,bok:100,ang:0,color:'#ff0'}),
		BALL = circle({x:285,y:286,r0:50,color:'#fff'}),
		RECT = rectangle({a:6.5,b:200,x:300,y:15,ang:pi4,color:'#00f'}),
		ObjRadioList,
		$ParamsInputList,
		checkedOne = "Circle",
		CallDrawing = false,
		Data = { Arrays:["Squares", "Players", "Circles", "Rectangles", "Entrances", "Exits", "Switches", "Doors", "Particles"],
					Squares:	Array(),
					Rectangles:	Array(),
					Circles:	Array(),
					Players:	Array(),
					Entrances:	Array(),
					Exits:	Array(),
					Switches:	Array(),
					Doors:	Array(),
					Particles:	Array(),
					Add:	AddObjectToData,
					Selected:	[],
					LastSelected:	[],
					selectedCentroid: null,
					Clicked:	{},
					IsDragging:	false,
					isClicked:	false,
					Background:	{Path: "default", Img: new Image()},
					mouseDragX:	0,
					mouseDragY:	0,
					lastMouseEvent: null,
					$selectRect:	null,
					$selectCtx:	null,
					$secondCanva:	null,
					$secondCtx:	null,
					rotate:	false,
					userRotPoint: {x: 0, y: 0},
					rotationPoint: null,
					transArgs: null,
					$rotCheck:	null,
					$callDrawCheck:	null,
					selectPoint: false,
					$radRotIndie: null,
					$radRotPoint: null,
					$radRotCentroid: null,
					$radRotUserPoint: null,
					$userPointX: null,
					$userPointY: null,
					$canvaCentralText: null,
					$thirdCanva: null,
					$thirdCtx: null
			};

if (document.addEventListener) {
  document.addEventListener('DOMContentLoaded',	function(){
	init(Data);
														},
														false);
}

function init(Module){
	if (arguments.callee.done) return;
	arguments.callee.done = true;
	var shiftDown = false;
	
	function startSelectPoint(){
		Module.selectPoint = true;
		Module.$canvaCentralText.style.left = $canva.width * 0.23 + "px";
		Module.$canvaCentralText.textContent = "Click on the map to select point";
		clearSecondCanva(Module);
		drawModuleCentroid(Module, 'fill');
	};

	document.onkeydown = function(e){
		if(!shiftDown){
			if(!e){
				e = window.event;
			}

			switch(e.keyCode){
				case 16://shift
					shiftDown = true;
					if(Module.selectedCentroid && !Module.selectPoint){
						drawTransformedSelection(Module, 'stroke', copyColor);
					}
					break;
			}
		}
	};
	document.onkeypress = function(e){
		var color;
		if(!Module.rotate){
			if(!e){
				e = window.event;
			}
//Module.lastMouseEvent = {x: evX, y: evY};
			switch(e.keyCode){
				case 82://r
				color = copyColor;
				case 114:
					Module.rotate = true;
					if(Module.selectedCentroid && !Module.selectPoint){
						if(Module.isClicked){
							Module.getLastMouseArgs();
						}else{
							Module.transArgs = {ang: 0.0000001};
						}
						Module.$selectRect.width = 0;
						Module.$selectRect.height = 0;
						if(color == null){
							color = SelectionColor;
						}
						drawTransformedSelection(Module, 'stroke', color);
					}
				break;
			}
		}
	};
	document.onkeyup = function(e){
		var color;
		if(!e){
			e = window.event;
		}

		switch(e.keyCode){
			// case 17://ctrl

			case 46://shift + del
			case 68://shift + d
				if(e.shiftKey && !Module.selectPoint){
					delObject();
				}
				break;
			case 67://shift + c
				if(e.shiftKey && !Module.selectPoint){
					cloneObject(Module, 'move', {x: -30, y: 30});
				}
				break;
			case 78://shift + n
				if(!e.shiftKey){
					break;
				}
			case 107://+
				if(!Module.selectPoint){
					newObject();
				}
				break;
			case 76://shift + l
				if(e.shiftKey && !Module.selectPoint){
					Load(Module);
				}
				break;
			case 83://shift + s
				if(e.shiftKey && !Module.selectPoint){
					GenerateOutput(Module);
				}
				break;
			case 16: //shift
				shiftDown = false;
				if(!Module.selectPoint){
					drawTransformedSelection(Module, 'stroke', SelectionColor);
				}
				break;
			case 86://shift + v
				if(e.shiftKey && !Module.selectPoint){
					startSelectPoint();
				}
				break;
			case 82://r
				Module.rotate = false;
				if(!Module.selectPoint){
					if(Module.isClicked){
						if(Module.selectedCentroid && Module.IsDragging){
							Module.getLastMouseArgs();
							if(shiftDown){
								color = copyColor;
							}else{
								color = SelectionColor;
							}
							drawTransformedSelection(Module, 'stroke', color);
						}else{
							drawSelectRect(Module);
							clearSecondCanva(Module);
							drawModuleCentroid(Module, 'fill');
						}
					}else{
						clearSecondCanva(Module);
						drawModuleCentroid(Module, 'fill');
					}
				}
				break;
			case 97://numbers on numeric keyboard
				chooseObjType('Circle');
				break;
			case 98:
				chooseObjType('Square');
				break;
			case 99:
				chooseObjType('Rectangle');
				break;
			case 100:
				chooseObjType('Player');
				break;
			case 101:
				chooseObjType('Entrance');
				break;
			case 102:
				chooseObjType('Exit');
				break;
			case 103:
				chooseObjType('Switch');
				break;
			case 104:
				chooseObjType('Door');
				break;
			case 105:
				chooseObjType('Particle');
				break;
		}
	};
	
	document.onmousedown = function(e){
		if(!Module.selectPoint){
			selectObjOnCanvas(e, Module);
		}
	};
	document.onmouseup = function(e){
		if(Module.selectPoint){
			if(e.pageX < $canva.width &&
				e.pageY < $canva.height){
				Module.userRotPoint.x = e.pageX;
				Module.userRotPoint.y = e.pageY;
				Module.$userPointX.value = Module.userRotPoint.x;
				Module.$userPointY.value = Module.userRotPoint.y;
				drawUserRotationPoint(Module);
				Module.setRotationPoint();
				Module.$canvaCentralText.textContent = "";
				Module.selectPoint = false;
			}
		}else{
			dropOnCanvas(e, Module);
		}
	};
	document.onmousemove = function(e){
		if(!Module.selectPoint){
			drawAtMouseMove(e, Module);
		}
	};
	
	Module.$rotCheck = gEBI("rotateCheck");
	Module.$callDraw = gEBI("callDraw");
	Module.$callDraw.onclick = function(){
		ReDraw(Module);
	};

	$BackgroundText = gEBI('BackgroundText');
	$ObjListSelect = $('#ObjListSelect');
	$ObjListParent = $('#ObjListParent');
	$OptGroups.$OptPlayers = $('#OptPlayers');
	$OptGroups.$OptCircles = $('#OptCircles');
	$OptGroups.$OptRectangles = $('#OptRectangles');
	$OptGroups.$OptSquares = $('#OptSquares');
	$OptGroups.$OptEntrances = $('#OptEntrances');
	$OptGroups.$OptExits = $('#OptExits');
	$OptGroups.$OptSwitches = $('#OptSwitches');
	$OptGroups.$OptDoors = $('#OptDoors');
	$OptGroups.$OptParticles = $('#OptParticles');
	$firstP = $('#firstP');
	$secondP = $('#secondP');
	$thirdP = $('#thirdP');
	$forthP = $('#forthP');
	$fifthP = $('#fifthP');
	$sixthP = $('#sixthP');
	$seventhP = $('#seventhP');
	$angleP = $('#angleP');
	$r0P = $('#r0P');
	$colorP = $('#colorP');
	$massP = $('#massP');
	
	Module.$radRotIndie = gEBI('radRotIndie');
	Module.$radRotPoint = gEBI('radRotPoint');
	Module.$radRotCentroid = gEBI('radRotCentroid');
	Module.$radRotUserPoint = gEBI('radRotUserPoint');
	Module.$radRotIndie.onchange = function(){
		Module.rotationPoint = 'indie';
	};
	Module.$radRotPoint.onchange = function(){
		if(Module.$radRotCentroid.checked){
			Module.$radRotCentroid.onchange();
		}else{
			Module.$radRotUserPoint.onchange();
		}
	};
	Module.$radRotCentroid.onchange = function(){
		Module.$radRotPoint.checked = true;
		Module.rotationPoint = Module.selectedCentroid;
	};
	Module.$radRotUserPoint.onchange = function(){
		Module.$radRotPoint.checked = true;
		Module.rotationPoint = Module.userRotPoint;
		drawUserRotationPoint(Module);
	};
	Module.setRotationPoint = function(){
		if(Module.$radRotIndie.checked){
			Module.$radRotIndie.onchange();
		}else{
			Module.$radRotPoint.onchange();
		}
	};

	$disp = gEBI('disp');
	$canva = gEBI('canva');
	$canva.width = wdth;
	$canva.height = hght;

	Module.$selectRect = gEBI('selectRect');
	Module.$selectRect.width = 0;
	Module.$selectRect.height = 0;
	Module.$selectRect.style.backgroundColor = selectRectColor;
	Module.$selectCtx = Module.$selectRect.getContext('2d');
	
	Module.$secondCanva = gEBI('secondCanva');
	Module.$secondCanva.width = $canva.width;
	Module.$secondCanva.height = $canva.height;
	Module.$secondCanva.style.backgroundColor = 'rgba(0, 0, 0, 0)';
	Module.$secondCtx = Module.$secondCanva.getContext('2d');
	Module.$secondCtx.lineWidth = 2;
	Module.$secondCtx.strokeStyle = SelectionColor;
	
	Module.$thirdCanva = gEBI('thirdCanva');
	Module.$thirdCanva.width = $canva.width;
	Module.$thirdCanva.height = $canva.height;
	Module.$thirdCanva.style.backgroundColor = 'rgba(0, 0, 0, 0)';
	Module.$thirdCtx = Module.$thirdCanva.getContext('2d');
	Module.$thirdCtx.lineWidth = 2;
	
	$ctx = $canva.getContext('2d');
	$ImgD = $ctx.getImageData(0,0,wdth-1,hght-1);
	
	$ctx.lineWidth = 1.6;
	$ctx.font = '25px Verdana';
	
	Module.$canvaCentralText = gEBI("canvaCentralText");
	Module.$canvaCentralText.style.top = $canva.height * 0.45 + "px";
	
	ObjRadioList = Array();
	ObjRadioList['Circle'] = gEBI('objTypeCircle');
	ObjRadioList['Square'] = gEBI('objTypeSquare');
	ObjRadioList['Rectangle'] = gEBI('objTypeRectangle');
	ObjRadioList['Player'] = gEBI('objTypePlayer');
	ObjRadioList['Entrance'] = gEBI('objTypeEntrance');
	ObjRadioList['Exit'] = gEBI('objTypeExit');
	ObjRadioList['Switch'] = gEBI('objTypeSwitch');
	ObjRadioList['Door'] = gEBI('objTypeDoor');
	ObjRadioList['Particle'] = gEBI('objTypeParticle');
	
	$sieczka = gEBI('sieczka');
	$SpanObjType = gEBI('SpanObjType');
	$ParamsInputList = Array();
	$ParamsInputList['First'] = gEBI('FirstText');
	$ParamsInputList['Second'] = gEBI('SecondText');
	$ParamsInputList['Third'] = gEBI('ThirdText');
	$ParamsInputList['Forth'] = gEBI('ForthText');
	$ParamsInputList['Fifth'] = gEBI('FifthText');
	$ParamsInputList['Sixth'] = gEBI('SixthText');
	$ParamsInputList['Seventh'] = gEBI('SeventhText');
	$ParamsInputList['Angle'] = gEBI('AngleText');
	$ParamsInputList['r0'] = gEBI('r0Text');
	$ParamsInputList['Color'] = gEBI('ColorText');
	$ParamsInputList['Mass'] = gEBI('MassText');
	
	Module.$userPointX = gEBI("userPointX");
	Module.$userPointY = gEBI("userPointY");
	Module.$userPointX.onchange = function(){
		var value = Module.$userPointX.value - 0;
		if(isNaN(value)){
			Module.$userPointX.value = Module.userRotPoint.x;
		}else{
			Module.userRotPoint.x = value;
			drawUserRotationPoint(Module);
		}
	};
	Module.$userPointY.onchange = function(){
		var value = Module.$userPointY.value - 0;
		if(isNaN(value)){
			Module.$userPointY.value = Module.userRotPoint.y;
		}else{
			Module.userRotPoint.y = value;
			drawUserRotationPoint(Module);
		}
	};
	gEBI("setUserPoint").onclick = startSelectPoint;

	//This will go away...
		
		addObject(SQR);
		addObject(RECT);
		addObject(BALL);
		addObject(player({x:100,y:300,ang:0}));
		for(var i = 1; i <= 100; i+=1){
			addObject(square({x: 5*i, 
								y: 2.5*i+2.5*i*Math.sin(i), 
								bok: 5+3*Math.sin(i),
								ang: i,
								color: '#000'}
								));
		}

		Module.transArgs = {x: 0, y: 0, ang: 0};
		CallDrawing = true;
		ListOut();
		SetSelectedOnListFromData();
		ShowObj(Module.Selected);
	//...up to here

	var mouseDownArg;
	Module.computeMouseDownArgs = function(x, y){
		var point;
		if(this.rotationPoint === 'indie'){
			point = this.selectedCentroid;
		}else{
			point = this.rotationPoint;
		}
		this.mouseDragX = x;
		this.mouseDragY = y;
		mouseDownArg = VectorAtan(x - point.x, y - point.y);
	};
	Module.computeTransArgs = function(x, y){
		var point;
		if(this.rotate){
			if(this.rotationPoint === 'indie'){
				point = this.selectedCentroid;
			}else{
				point = this.rotationPoint;
			}
			this.transArgs = {ang: VectorAtan(x - point.x, y - point.y) - mouseDownArg};
		}else{
			this.transArgs = {x: x - this.mouseDragX,
			y: y - this.mouseDragY};
		}
	};
	Module.getLastMouseArgs = function(){
		var p = Module.lastMouseEvent;
		Module.computeTransArgs(p.x, p.y);
	};

	drawUserRotationPoint(Module);
	ReDraw(Module);
}

//-------------------------------------------------------------

function gEBI(e){
	return document.getElementById(e);
}

function roundToTwo(FloatNumber){
	FloatNumber += "";
	var commaPos = FloatNumber.indexOf('.');
	if(commaPos === -1){
		return FloatNumber;
	}else{
		FloatNumber = FloatNumber.slice(0, commaPos + 3);
		if(FloatNumber[FloatNumber.length - 1] === "0"){
			if(FloatNumber[FloatNumber.length - 2] === "0"){
				FloatNumber = FloatNumber.slice(0, FloatNumber.length - 3);
			}
			else{
				FloatNumber = FloatNumber.slice(0, FloatNumber.length - 1);
			}
		}
		return FloatNumber;
	}
}

//-------------------------------------------------------------

function GenerateOutput(Module){
	var op, i, j;
	if(Module.Background.Path === "default"){
		$sieczka.value = "0\n";
	}else{
		$sieczka.value = Module.Background.Path + "\n";
	}
	
	$sieczka.value += Module.Players.length + "\n";
	for(i = 0; i < Module.Players.length; ++i){
		$sieczka.value += roundToTwo(Module.Players[i].x) + " " + 
			  roundToTwo(Module.Players[i].y) + " " + 
			  roundToTwo(Module.Players[i].ang) +
			  "\n";
	}

	$sieczka.value += Module.Rectangles.length + "\n";
	for(i = 0; i < Module.Rectangles.length; ++i){
		$sieczka.value += roundToTwo(Module.Rectangles[i].x) + " " + 
			  roundToTwo(Module.Rectangles[i].y) + " " + 
			  roundToTwo(Module.Rectangles[i].a) + " " + 
			  roundToTwo(Module.Rectangles[i].b) + " " + 
			  roundToTwo(Module.Rectangles[i].ang) + " " +
				ColorToNumbers(Module.Rectangles[i].color) +
			  "\n";
	}
	
	$sieczka.value += Module.Circles.length + "\n";
	for(i = 0; i < Module.Circles.length; ++i){
		$sieczka.value += roundToTwo(Module.Circles[i].x) + " " + 
			  roundToTwo(Module.Circles[i].y) + " " +
			  roundToTwo(Module.Circles[i].r0) + " " +
				ColorToNumbers(Module.Circles[i].color) +
			  "\n";
	}
	
	$sieczka.value += Module.Squares.length + "\n";
	for(i = 0; i < Module.Squares.length; ++i){
		$sieczka.value += roundToTwo(Module.Squares[i].x) + " " +
			  roundToTwo(Module.Squares[i].y) + " " +
			  roundToTwo(Module.Squares[i].bok) + " " +
			  roundToTwo(Module.Squares[i].ang) + " " +
				ColorToNumbers(Module.Squares[i].color) +
			  "\n";
	}
	
	$sieczka.value += Module.Entrances.length + "\n";
	for(i = 0; i < Module.Entrances.length; ++i){
		$sieczka.value += roundToTwo(Module.Entrances[i].x) + " " + 
			  roundToTwo(Module.Entrances[i].y) + " " + 
			  roundToTwo(Module.Entrances[i].a) + " " + 
			  roundToTwo(Module.Entrances[i].b) + " " + 
			  roundToTwo(Module.Entrances[i].ang) + " " +
				ColorToNumbers(Module.Entrances[i].color) +
			  "\n";
	}
	
	$sieczka.value += Module.Exits.length + "\n";
	for(i = 0; i < Module.Exits.length; ++i){
		$sieczka.value += roundToTwo(Module.Exits[i].x) + " " + 
			  roundToTwo(Module.Exits[i].y) + " " + 
			  roundToTwo(Module.Exits[i].a) + " " + 
			  roundToTwo(Module.Exits[i].b) + " " + 
			  roundToTwo(Module.Exits[i].ang) + " " +
				ColorToNumbers(Module.Exits[i].color) +
			  "\n";
	}

	$sieczka.value += Module.Switches.length + "\n";
	for(i = 0; i < Module.Switches.length; ++i){
		$sieczka.value += roundToTwo(Module.Switches[i].x) + " " + 
							  roundToTwo(Module.Switches[i].y) + " " + 
							  roundToTwo(Module.Switches[i].a) + " " + 
							  roundToTwo(Module.Switches[i].b) + " " + 
							  roundToTwo(Module.Switches[i].ang) + " " + 
								swTypeToNumber(Module.Switches[i].swType) + " " + 
								roundToTwo(Module.Switches[i].pos) + " " +
								Module.Switches[i].connected.Doors.length + "\n";

		Module.Switches[i].connected.Doors.sort(sortfunc);
		Module.Switches[i].connected.Switches.sort(sortfunc);
		op = "";
		for(j = 0; j < Module.Switches[i].connected.Doors.length; ++j){
			op += Module.Switches[i].connected.Doors[j] + " ";
		}
		if(op != ""){
			$sieczka.value += op.slice(0, -1) + "\n";
		}

		$sieczka.value += Module.Switches[i].connected.Switches.length + "\n";
		
		op = "";
		for(j = 0; j < Module.Switches[i].connected.Switches.length; ++j){
			op += Module.Switches[i].connected.Switches[j] + " ";
		}
		if(op != ""){
			$sieczka.value += op.slice(0, -1) + "\n";
		}
		
		$sieczka.value +=	roundToTwo(Module.Switches[i].mass) + " " +
											ColorToNumbers(Module.Switches[i].color) + "\n";
	}
	
	$sieczka.value += Module.Doors.length + "\n";
	for(i = 0; i < Module.Doors.length; ++i){
		$sieczka.value += roundToTwo(Module.Doors[i].x) + " " + 
			  roundToTwo(Module.Doors[i].y) + " " + 
			  roundToTwo(Module.Doors[i].a) + " " + 
			  roundToTwo(Module.Doors[i].b) + " " + 
			  roundToTwo(Module.Doors[i].ang) + " " +
				doorTypeToNumber(Module.Doors[i].doorType) + " " +
				roundToTwo(Module.Doors[i].pos) + " " +
			  roundToTwo(Module.Doors[i].openingTime) + " " +
				ColorToNumbers(Module.Doors[i].color) +
			  "\n";
	}

	$sieczka.value += Module.Particles.length + "\n";
	for(i = 0; i < Module.Particles.length; ++i){
		$sieczka.value += roundToTwo(Module.Particles[i].x) + " " + 
			  roundToTwo(Module.Particles[i].y) + " " + 
			  roundToTwo(Module.Particles[i].r0) + " " + 
			  roundToTwo(Module.Particles[i].mass) + " " + 
			  roundToTwo(Module.Particles[i].charge) +
			  "\n";
	}
}

function ColorToNumbers(color){
	var op, pos;
	color += "";
	if(color[0] === "#"){
		switch(color.length){
			case 4:
				return "3\n" + Shorth2d(color[1]) + " " + Shorth2d(color[2]) + " " + Shorth2d(color[3]);
			case 7:
				return "3\n" + (h2d(color[1]) * 16 + h2d(color[2])) + " " + (h2d(color[3]) * 16 + h2d(color[4])) + " " + (h2d(color[5]) * 16 + h2d(color[6]));
		}
	}
	switch(color.slice(0,4)){
		case "rgb(":
			op = color.slice(4, -1);
			pos = op.indexOf(",");
			color = "3\n" + (op.slice(0, pos) - 0) + " ";
			op = op.slice(pos + 1);
			pos = op.indexOf(",");
			color += op.slice(0, pos) - 0 + " ";
			op = op.slice(pos + 1);
			color += op - 0;
			return color;
		case "rgba":
			op = color.slice(5, -1);
			pos = op.indexOf(",");
			color = "4\n" + (op.slice(0, pos) - 0) + " ";
			op = op.slice(pos + 1);
			pos = op.indexOf(",");
			color += op.slice(0, pos) - 0 + " ";
			op = op.slice(pos + 1);
			pos = op.indexOf(",");
			color += op.slice(0, pos) - 0 + " ";
			op = op.slice(pos + 1);
			color += op - 0;
			return color;		
	}
	console.log("invalid color!");
	return color;
}

function numbersToColor(n, arr){
	switch(n){
		case "3":
			return 	"rgb(" +
							arr[0] + ", " +
							arr[1] + ", " +
							arr[2] + ")";
		case "4":
			return 	"rgba(" +
							arr[0] + ", " +
							arr[1] + ", " +
							arr[2] + ", " +
							arr[3] + ")";
	}
}

function doorTypeToNumber(doorType){
	switch(doorType){
		case '1wingSlide': return 0;
		default: return 0;
	}
}

function swTypeToNumber(swType){
	switch(swType){
		case 'slide': return 0;
		default: return 0;
	}
}

function numberToSwType(num){
	switch(num){
		case 0: return 'slide';
		default: return 'slide';
	}
}

function numberToDoorType(num){
	switch(num){
		case 0: return '1wingSlide';
		default: return '1wingSlide';
	}
}

function Load(Module){
	var input = $sieczka.value.split('\n'),
		 i,
		 j,
		 k,
		 op,
		 numOfSwitches,
		 drawing = CallDrawing,
		 connected;

	CallDrawing = false;
	k = input.length;
	for(i = 0; i < k; ++i){
		input[i] = input[i].split(' ');
	}
	
	k = Module.Arrays.length;
	for(i = 0; i < k; ++i){
		Module[Module.Arrays[i]] = [];
	}
	Module.Selected = [];
	Module.LastSelected = [];
	
	SetBackground(Module, input[0][0]);
	
	i = 1;
	for(j = i + 1; j <= i + (input[i][0] - 0); j += 1){
		Module.Add(player({	x: input[j][0] - 0,
												y: input[j][1] - 0,
												ang: input[j][2] - 0
												}
											));
	}
	
	i += (input[i][0] - 0) + 1;
	for(j = i + 1; j <= i + (input[i][0] - 0) * 2; j += 2){
		Module.Add(rectangle({x: input[j][0] - 0,
													y: input[j][1] - 0,
													a: input[j][2] - 0,
													b: input[j][3] - 0,
													ang: input[j][4] - 0,
													color: numbersToColor(input[j][5], input[j + 1])
													}
												));
	}
	
	i = j;
	for(j = i + 1; j <= i + (input[i][0] - 0) * 2; j += 2){
		Module.Add(circle({	x: input[j][0] - 0,
												y: input[j][1] - 0,
												r0: input[j][2] - 0,
												color: numbersToColor(input[j][3], input[j + 1])}
											));
	}
	
	i = j;
	for(j = i + 1; j <= i + (input[i][0] - 0) * 2; j += 2){
		Module.Add(square({	x: input[j][0] - 0,
												y: input[j][1] - 0,
												bok: input[j][2] - 0,
												ang: input[j][3] - 0,
												color: numbersToColor(input[j][4], input[j + 1])
											}
										));
	}
	
	i = j;
	for(j = i + 1; j <= i + (input[i][0] - 0) * 2; j += 2){
		Module.Add(entrance({	x: input[j][0] - 0,
													y: input[j][1] - 0,
													a: input[j][2] - 0,
													b: input[j][3] - 0,
													ang: input[j][4] - 0,
													color: numbersToColor(input[j][5], input[j + 1])
												}
											));
	}
	
	i = j;
	for(j = i + 1; j <= i + (input[i][0] - 0) * 2; j += 2){
		Module.Add(exit({	x: input[j][0] - 0,
											y: input[j][1] - 0,
											a: input[j][2] - 0,
											b: input[j][3] - 0,
											ang: input[j][4] - 0,
											color: numbersToColor(input[j][5], input[j + 1])
										}
									));
	}
	
	i = j;
	for(j = i + 1, numOfSwitches = 0;
			numOfSwitches < input[i][0] - 0;
			j += 2, numOfSwitches += 1){
		connected = {	Doors: [],
									Switches: []
								 };
		op = j + 1;
		console.log([j, input[j][7] - 0]);
		for(k = 0; k < input[j][7] - 0; k += 1){
			connected.Doors.push(input[op][k] - 0);
		}
		if(input[j][7] - 0){
			op += 1;
		}
		console.log(input[op][0] - 0);
		for(k = 0; k < input[op][0] - 0; k += 1){
			connected.Switches.push(input[op + 1][k] - 0);
		}
		if([op, input[op][0] - 0]){
			op += 1;
		}
		op += 1;
		connected.Doors.sort(sortfunc);
		connected.Switches.sort(sortfunc);
		console.log(connected);
		Module.Add(aswitch({x: input[j][0] - 0,
												y: input[j][1] - 0,
												a: input[j][2] - 0,
												b: input[j][3] - 0,
												ang: input[j][4] - 0,
												swType: numberToSwType(input[j][5] - 0),
												pos: input[j][6] - 0,
												connected: connected,
												mass: input[op][0] - 0,
												color: numbersToColor(input[op][1], input[op + 1])
												}
											));
		j = op;
	}
	
	i = j;
	for(j = i + 1; j <= i + (input[i][0] - 0) * 2; j += 2){
		Module.Add(door({	x: input[j][0] - 0,
											y: input[j][1] - 0,
											a: input[j][2] - 0,
											b: input[j][3] - 0,
											ang: input[j][4] - 0,
											doorType: numberToDoorType(input[j][5] - 0),
											pos: input[j][6] - 0,
											openingTime: input[j][7] - 0,
											color: numbersToColor(input[j][8], input[j + 1])
										}
									));
	}
	
	i = j;
	for(j = i + 1; j <= i + (input[i][0] - 0); j += 1){
		Module.Add(particle({	x: input[j][0] - 0,
													y: input[j][1] - 0, 
													r0: input[j][2] - 0,
													mass: input[j][3] - 0,
													charge: input[j][4] - 0
												}
											));
	}

		

	
	ListOut();
	SetSelectedOnListFromData();
	ShowObj(Module.Selected);
	CallDrawing = drawing;
	ReDraw(Module);
}

function Clear(){
	var i
		, len;
	
	len = Data.Arrays.length;
	for(i = 0; i < len; ++i){
		Data[Data.Arrays[i]] = [];
	}
	
	len = $OptGroups.length;
	for(i = 0; i < len; ++i){
		$OptGroups[i].empty();
	}
	Data.Selected = [];
	Data.LastSelected = [];
	ShowObj(Data.Selected);
	ReDraw(Data);
}

function delObject(){
	var Sel = GetSelectedOnList(),
	    GroupName,
		 last,
		 i, j;
	if(!(Sel == false)){
		GroupName = ObjNameToGroupName(Sel[0]);
		
		last = Data[GroupName].length - 1;
		for(i = Sel[1] - 0; i < last; i+=1){
			Data[GroupName][i] = Data[GroupName][i + 1];
		}
		Data[GroupName].length = last;

		$OptGroups["$Opt" + GroupName].children().last().empty().detach();
	}else{
		for(j = 0; j < Data.Selected.length; j+=1){
			if(Data.Selected[j] != undefined){
				GroupName = ObjNameToGroupName(Data.Selected[j].type);
				
				last = Data[GroupName].length - 1;
				
				for(i = 0; i <= last; i+=1){
					if(Data[GroupName][i] === Data.Selected[j]){
						break;
					}
				}
				
				for(; i < last; i+=1){
					Data[GroupName][i] = Data[GroupName][i + 1];
				}
				Data[GroupName].length = last;
				
				$OptGroups["$Opt" + GroupName].children().last().empty().detach();
			}
		}
		
	}
	Data.Selected = [];
	Data.selectedCentroid = null;
	Data.setRotationPoint();

	ReDraw(Data);
	SetSelectedOnListFromData();
	ShowObj(Data.Selected);
}

function AddObjectToData(Obj){
	if(Obj.type != undefined){
		return this[ObjNameToGroupName(Obj.type)].push(Obj) - 1;
	}
	//console.log(['No such object as: ', Obj.type]);
}

function addObject(newObj){
	AddToList(newObj, Data.Add(newObj));
	Data.LastSelected = Data.Selected;
	Data.Selected = [newObj];
	Data.selectedCentroid = centroid({x: newObj.x, y: newObj.y});
	Data.setRotationPoint();
	ShowObj(Data.Selected);
	SetSelectedOnListFromData();
	ReDraw(Data);
}

function AddToList(Obj, index){
	var GroupName;
	
	GroupName = "$Opt" + ObjNameToGroupName(Obj.type);
	$("<option></option>").text(GetSelectListNameFromObj(Obj, index)).appendTo($OptGroups[GroupName]);
}

function ListOut(){
	$ObjListSelect.detach();
	
	var i, j, len, len2, objName, groupName;
	
	len = Data.Arrays.length;
	for(i = 0; i < len; ++i){
		objName = GroupToObjName(Data.Arrays[i]);
		
		groupName = "$Opt" + Data.Arrays[i];
		$OptGroups[groupName].empty();
		
		for(j = 0, len2 = Data[Data.Arrays[i]].length; j < len2; ++j){
			$("<option></option>").text(objName + " " + j)
				.appendTo($OptGroups[groupName]);
		}
	}
	$ObjListParent.append($ObjListSelect);
}

function newObject(){
	var newObj;
	
	switch(checkedOne){
		case "Circle": 
			newObj = circle({x:wdth/2,y:hght/2,r0:50,color:'#fff'});
			break;
		case "Square":
			newObj = square({x:wdth/2,y:hght/2,bok:100,ang:0,color:'#fff'});
			break;
		case "Rectangle":
			newObj = rectangle({a:30,b:200,x:wdth/2,y:hght/2,ang:0,color:'#fff'});
			break;
		case "Player":
			newObj = player({x:wdth/2,y:hght/2,ang:0});
			break;
		case "Entrance":
			newObj = entrance({a:30,b:40,x:wdth/2,y:hght/2,ang:0,color:'#fff'});
			break;
		case "Exit":
			newObj = exit({a:30,b:40,x:wdth/2,y:hght/2,ang:0,color:'#fff'});
			break;
		case "Switch":
			newObj = aswitch({a:60,b:30,x:wdth/2,y:hght/2,ang:0,color:'#fff',swType:"slide",pos:0,connected:{Doors:[],Switches:[]},mass:1});
			break;
		case "Door":
			newObj = door({a:30,b:60,x:wdth/2,y:hght/2,ang:0,color:'#fff',pos:0,doorType:"1wingSlide",openingTime:1000});
			break;
		case "Particle":
			newObj = particle({x:wdth/2,y:hght/2,ang:0,r0:10,charge:-500,mass:1});
			break;
	}
	
	addObject(newObj);	
}

function cloneObject(Module, transType, args){
	var newObj
		, drawing = CallDrawing
		, i
		,	len;
	
	len = Module.Selected.length;
	for(i = 0; i < len; ++i){//to powinno stąd wypaść...
		Module.Selected[i].draw($ctx, 'fill');
	}
	Module.LastSelected = Module.Selected;
	Module.Selected = [];
	CallDrawing = false;
	
	
	if(Module.selectedCentroid){
		cloneTransformedSelection(Module, transType, args);
		if(transType === 'move'){
			movePoint(Module.selectedCentroid, args.x, args.y);
		}
	}
	
	ShowObj(Module.Selected);
	SetSelectedOnListFromData();
	CallDrawing = drawing;
	ReDraw(Module);
}

function cloneTransformedSelection(Module, transType, args){
	function clone(Obj){
		var newObj;
		if(Obj){
			switch(Obj.type){
				case "Circle": 
					newObj = circle;
					break;
				case "Square":
					newObj = square;
					break;
				case "Rectangle":
					newObj = rectangle;
					break;
				case "Player":
					newObj = player;
					break;
				case "Entrance":
					newObj = entrance;
					break;
				case "Exit":
					newObj = exit;
					break;
				case "Switch":
					newObj = aswitch;
					break;
				case "Door":
					newObj = door;
					break;
				case "Particle":
					newObj = particle;
					break;	
			}
			newObj = newObj(Obj);
			AddToList(newObj, Module.Add(newObj));
			Module.Selected.push(newObj);
		}
	}
	
	if(transType == null){
		if(Module.rotate){
			transType = 'turn';
		}else{
			transType = 'move';
		}
		args = Module.transArgs;
	}
	
	switch(transType){
		case 'turn':
			turnArray(Module.LastSelected, Module.rotationPoint, args.ang, clone);
			break;
		case 'move':
			moveArray(Module.LastSelected, args.x, args.y, clone);
			break;
	}
	Module.selectedCentroid = findCentroid(Module.Selected);
	Module.setRotationPoint();
}

//-------------------------------------------------------------

function SelectObject(){
	var Obj = GetObjectFromSelection(GetSelectedOnList());
	
	Data.LastSelected = Data.Selected;
	Data.Selected = [Obj];
	Data.selectedCentroid = centroid({x: Obj.x, y: Obj.y});
	Module.setRotationPoint();
	ShowObj(Data.Selected);
	
	drawSelection();
}

function GetObjectIndex(Selected){
	var i, group = ObjNameToGroupName(Selected.type);
	for(i = 0; i < Data[group].length; i+=1){
		if(Selected === Data[group][i]){
			return i;
			}
	}
	return -1;
}

function GetSelectedOnList(){
	if($ObjListSelect[0].value === "")
		return [];
	return $ObjListSelect[0].value.split(' ');
}

function SetSelectedOnListFromData(){
	if(Data.Selected.length === 1){
		for(var index = 0; index < Data[ObjNameToGroupName(Data.Selected[0].type)].length; index+=1){
			if(Data.Selected[0] === Data[ObjNameToGroupName(Data.Selected[0].type)][index]){
				break;
				}
		}
		var index = GetObjectIndex(Data.Selected[0]);
		if(index === -1){
			$ObjListSelect[0].value = "";
		}else{
			$ObjListSelect[0].value = Data.Selected[0].type + " " + index;
		}
	}else{
		$ObjListSelect[0].value = "";
	}
}

function GetObjectFromSelection(SelectionArray){
	return Data[ObjNameToGroupName(SelectionArray[0])][SelectionArray[1]];
}

function GroupToObjName(GroupName){
	switch(GroupName){
			case "Switches":
				return "Switch";
			default:
				return GroupName.slice(0, GroupName.length - 1);
		}
}

function ObjNameToGroupName(ObjName){
	switch(ObjName){
		case "Switch":
			return "Switches";
		default:
			return ObjName + "s";
	}
}

function GetSelectListNameFromObj(Obj, index){
	return Obj.type + " " + index;
}

function drawSelection(){
	var i
		,	len;
	
	len = Data.LastSelected.length;
	for(i = 0; i < len; ++i){
		if(Data.LastSelected[i].draw){
			Data.LastSelected[i].draw($ctx, 'fill');
		}
	}
	
	len = Data.Selected.length;
	for(i = 0; i < len; ++i){
		if(Data.Selected[i].draw){
			Data.Selected[i].draw($ctx, 'fill', SelectionColor);
		}
	}
	
	clearSecondCanva(Data);
	drawModuleCentroid(Data, 'fill');
}

function ShowObj(Objects){
	var Obj
		,	i
		,	len;
	if(Objects.length > 0){
		Obj = Objects[0];
		
		$SpanObjType.textContent = Obj.type;
		$ParamsInputList['First'].value = Obj.x;
		$ParamsInputList['Second'].value = Obj.y;
		$ParamsInputList['Angle'].value = Obj.ang;
		$ParamsInputList['r0'].value = Obj.r0;
		$ParamsInputList['Color'].value = Obj.color;
		$ParamsInputList['Mass'].value = Obj.mass;
		
		$massP.hide();
		$colorP.show();
		$angleP.show();
		$fifthP.hide();
		$sixthP.hide();
		$seventhP.hide();
		
		switch(Obj.type){
			case 'Square':
				setThird(Obj.bok, "bok");
				$thirdP.show();
				$forthP.hide();
				break;
			case 'Rectangle':
				setThird(Obj.a, "a");
				setForth(Obj.b, "b");
				setFifth(Obj.fi0, "fi<sub>0</sub>");
				setSixth(Obj.wsp1, "wsp<sub>1</sub>");
				setSeventh(Obj.wsp2, "wsp<sub>2</sub>");
				$thirdP.show();
				$forthP.show();
				$fifthP.show();
				$sixthP.show();
				$seventhP.show();
				break;
			case 'Entrance':
				setThird(Obj.a, "a");
				setForth(Obj.b, "b");
				setFifth(Obj.fi0, "fi<sub>0</sub>");
				setSixth(Obj.wsp1, "wsp<sub>1</sub>");
				setSeventh(Obj.wsp2, "wsp<sub>2</sub>");
				$fifthP.show();
				$sixthP.show();
				$seventhP.show();
				$thirdP.show();
				$forthP.show();
				break;
			case 'Exit':
				setThird(Obj.a, "a");
				setForth(Obj.b, "b");
				setFifth(Obj.fi0, "fi<sub>0</sub>");
				setSixth(Obj.wsp1, "wsp<sub>1</sub>");
				setSeventh(Obj.wsp2, "wsp<sub>2</sub>");
				$thirdP.show();
				$forthP.show();
				$fifthP.show();
				$sixthP.show();
				$seventhP.show();
				break;
			case 'Player':
				$thirdP.hide();
				$forthP.hide();
				$colorP.hide();
				break;
			case 'Circle':
				$thirdP.hide();
				$forthP.hide();
				$angleP.hide();
				break;
			case 'Particle':
				setThird(Obj.charge, "charge");
				$thirdP.show();
				$forthP.hide();
				$angleP.hide();
				$massP.show();
				break;
			case 'Switch':
				setThird(Obj.a, "a");
				setForth(Obj.b, "b");
				setFifth(Obj.swType, "swType");
				setSixth(Obj.pos, "pos");
				setSeventh(JSON.stringify(Obj.connected), "connected");
				$thirdP.show();
				$forthP.show();
				$fifthP.show();
				$sixthP.show();
				$seventhP.show();
				$massP.show();
				break;
			case 'Door':
				setThird(Obj.a, "a");
				setForth(Obj.b, "b");
				setFifth(Obj.doorType, "doorType");
				setSixth(Obj.pos, "pos");
				setSeventh(Obj.openingTime, "openingTime");
				$thirdP.show();
				$forthP.show();
				$fifthP.show();
				$sixthP.show();
				$seventhP.show();
				break;
		}
	}
	else{
		$ObjListSelect[0].value = 'none';
		
		len = $ParamsInputList.length;
		for(i = 0; i < len; ++i){
			$ParamsInputList[i].value = '';
			
		}
		$SpanObjType.textContent = 'Nothing';
	}
	//console.log(Obj);
}

//-------------------------------------------------------------

function Change(which){
	var NewValue = $ParamsInputList[which].value
		,	i
		,	j
		,	len;
	if(Data.Selected.length > 0){
		switch(which){
			case 'First':
				NewValue -= Data.Selected[0].x;
				transformSelection(Data, {x: NewValue, y: 0, type: 'move'});
				break;
			case 'Second':
				NewValue -= Data.Selected[0].y;
				transformSelection(Data, {x: 0, y: NewValue, type: 'move'});
				break;
			case 'Third':
				switch(Data.Selected[0].type){
					case 'Square': 
						Data.Selected[0].bok = NewValue - 0;
						Data.Selected[0] = square(Data.Selected[0]);
						Data.Squares[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Rectangle':
						Data.Selected[0].a = NewValue - 0;
						Data.Selected[0] = rectangle(Data.Selected[0]);
						Data.Rectangles[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Entrance':
						Data.Selected[0].a = NewValue - 0;
						Data.Selected[0] = entrance(Data.Selected[0]);
						Data.Entrances[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Exit':
						Data.Selected[0].a = NewValue - 0;
						Data.Selected[0] = exit(Data.Selected[0]);
						Data.Exits[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Switch':
						Data.Selected[0].a = NewValue - 0;
						Data.Selected[0] = aswitch(Data.Selected[0]);
						Data.Switches[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Door':
						Data.Selected[0].a = NewValue - 0;
						Data.Selected[0] = door(Data.Selected[0]);
						Data.Doors[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Particle':
						NewValue -= 0;
						len = Data.Selected.length;
						for(i = 0; i < len; ++i){
							if(Data.Selected[i].type === 'Particle'){
								Data.Selected[i].charge = NewValue;
								j = GetObjectIndex(Data.Selected[i]);
								Data.Selected[i] = particle(Data.Selected[i]);
								Data.Particles[j] = Data.Selected[i];
							}
						}
						break;
				}
				break;
			case 'Forth':
				switch(Data.Selected[0].type){
					case 'Rectangle':
						Data.Selected[0].b = NewValue - 0;
						Data.Selected[0] = rectangle(Data.Selected[0]);
						Data.Rectangles[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Entrance':
						Data.Selected[0].b = NewValue - 0;
						Data.Selected[0] = entrance(Data.Selected[0]);
						Data.Entrances[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Exit':
						Data.Selected[0].b = NewValue - 0;
						Data.Selected[0] = exit(Data.Selected[0]);
						Data.Exits[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Switch':
						Data.Selected[0].b = NewValue - 0;
						Data.Selected[0] = aswitch(Data.Selected[0]);
						Data.Switches[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Door':
						Data.Selected[0].b = NewValue - 0;
						Data.Selected[0] = door(Data.Selected[0]);
						Data.Doors[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
				}
				break;
			case 'Fifth':
				switch(Data.Selected[0].type){
					case 'Switch': 
						Data.Selected[0].swType = NewValue;
						Data.Selected[0] = aswitch(Data.Selected[0]);
						Data.Switches[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Door':
						Data.Selected[0].doorType = NewValue;
						Data.Selected[0] = door(Data.Selected[0]);
						Data.Doors[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
				}
				break;
			case 'Sixth':
				switch(Data.Selected[0].type){
					case 'Switch': 
						Data.Selected[0].pos = NewValue - 0;
						Data.Selected[0] = aswitch(Data.Selected[0]);
						Data.Switches[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Door': 
						Data.Selected[0].pos = NewValue - 0;
						Data.Selected[0] = door(Data.Selected[0]);
						Data.Doors[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
				}
				break;
			case 'Seventh':
				switch(Data.Selected[0].type){
					case 'Switch': 
						Data.Selected[0].connected = JSON.parse(NewValue);
						Data.Selected[0].connected.Doors.sort(sortfunc);
						Data.Selected[0].connected.Switches.sort(sortfunc);
						Data.Selected[0] = aswitch(Data.Selected[0]);
						Data.Switches[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Door': 
						Data.Selected[0].openingTime = NewValue - 0;
						break;
				}
				break;
			case 'r0':
				switch(Data.Selected[0].type){
					case 'Square':
						Data.Selected[0].bok = sqrt2 * (NewValue - 0);
						Data.Selected[0] = square(Data.Selected[0]);
						Data.Squares[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Circle':
						Data.Selected[0].r0 = NewValue - 0;
						Data.Selected[0] = circle(Data.Selected[0]);
						Data.Circles[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
					case 'Particle':
						Data.Selected[0].r0 = NewValue - 0;
						Data.Selected[0] = particle(Data.Selected[0]);
						Data.Particles[GetSelectedOnList()[1]] = Data.Selected[0];
						break;
				}
				break;
			case 'Angle':
				NewValue -= 0;
				NewValue = norm(NewValue);
				transformSelection(Data, {ang: NewValue - Data.Selected[0].ang, type: 'turn'});
				break;
			case 'Color':
				len = Data.Selected.length;
				for(i = 0; i < len; ++i){
					if( (Data.Selected[i].type != "Player") && 
					 (Data.Selected[i].type != "Particle") ){
						Data.Selected[i].color = NewValue;
					}
				}
				break;
			case 'Mass':
				len = Data.Selected.length;
				for(i = 0; i < len; ++i){
					if( (Data.Selected[i].type == "Particle") ||
						 (Data.Selected[i].type == "Switch") ){
						Data.Selected[i].mass = NewValue;
					}
				}
				break;
		}
		
		
		ShowObj(Data.Selected);
		ReDraw(Data);
	}
}

function setThird(text, etiq){
	var $temp;
	$ParamsInputList['Third'].value = text;
	$temp = $thirdP.children().detach().last();
	$thirdP.html(etiq).append($temp);
}

function setForth(text, etiq){
	var $temp;
	$ParamsInputList['Forth'].value = text;
	$temp = $forthP.children().detach().last();
	$forthP.html(etiq).append($temp);
}

function setFifth(text, etiq){
	var $temp;
	$ParamsInputList['Fifth'].value = text;
	$temp = $fifthP.children().detach().last();
	$fifthP.html(etiq).append($temp);
}

function setSixth(text, etiq){
	var $temp;
	$ParamsInputList['Sixth'].value = text;
	$temp = $sixthP.children().detach().last();
	$sixthP.html(etiq).append($temp);
}

function setSeventh(text, etiq){
	var $temp;
	$ParamsInputList['Seventh'].value = text;
	$temp = $seventhP.children().detach().last();
	$seventhP.html(etiq).append($temp);
}


//-------------------------------------------------------------

function ClearCanvas(Module){
	$ctx.fillStyle='rgba(255,128,0,255)';
	$ctx.fillRect(0,0,wdth-1,hght-1);
	$ctx.fillStyle='rgba(0,0,0,0.6)';
	$ctx.fillRect(0,0,wdth-1,hght-1);
	if(Module.Background.Path != "default"){
		$ctx.drawImage(Module.Background.Img, 0, 0);
	}
}

function ReDraw(Module){
	var i
		,	j
		,	len
		,	len2;
	if(CallDrawing){
		ClearCanvas(Module);
	
		len = Module.Arrays.length;
		for(i = 0; i < len; ++i){
			len2 = Module[Module.Arrays[i]].length;
			for(j = 0; j < len2; ++j){
				Module[Module.Arrays[i]][j].draw($ctx, 'fill');
			}
		}
		len = Module.Selected.length;
		for(i = 0; i < len; ++i){
			Module.Selected[i].draw($ctx, 'fill', SelectionColor);
		}

		clearSecondCanva(Module);
		drawModuleCentroid(Module, 'fill');
	}
}

function SetBackground(Module, Path){
	Module.Background.Img.src = Path;
	Module.Background.Img.onload = function(){
		if(Module.Background.Img.width > 0){
			Module.Background.Path = Path;
		}else{
			Module.Background.Path = "default";
		}
		$BackgroundText.value = Module.Background.Path
		ReDraw(Module);
	}
	Module.Background.Img.onerror = function(){
		Module.Background.Path = "default";
		$BackgroundText.value = "default";
		Module.Background.Img.src = null;
		ReDraw(Module);
	}
}

//-------------------------------------------------------------

function sqr(r0,fi){
	fi -= pi4;
	var sin=Math.sin(fi),
		 cos=Math.cos(fi);
	sin = (sin + sign(sin * cos) * cos);
	if(sin === 0){
		sin = 1;
	}
	return abs(r0 / sin);
}

function findRectVertices(Rec){
	var fi = pipol - Rec.ang + Rec.fi0 / 2,
		 verts = [{x:0, y:0}, {x:0, y:0}, {x:0, y:0}, {x:0, y:0}];
	verts[0].x = Rec.x + Rec.r0 * Math.cos(fi);
	verts[0].y = Rec.y + Rec.r0 * Math.sin(fi);
	fi += pi;
	verts[2].x = Rec.x + Rec.r0 * Math.cos(fi);
	verts[2].y = Rec.y + Rec.r0 * Math.sin(fi);
	fi = pipol - Rec.ang - Rec.fi0 / 2;
	verts[1].x = Rec.x + Rec.r0 * Math.cos(fi);
	verts[1].y = Rec.y + Rec.r0 * Math.sin(fi);
	fi += pi;
	verts[3].x = Rec.x + Rec.r0 * Math.cos(fi);
	verts[3].y = Rec.y + Rec.r0 * Math.sin(fi);
	return verts;
}

function findSquareVertices(Sqr){
	var verts = [{x:0, y:0}, {x:0, y:0}, {x:0, y:0}, {x:0, y:0}],
		 fi = pi4 + Sqr.ang;
	verts[0].x = Sqr.x + Sqr.r0 * Math.cos(fi);
	verts[0].y = Sqr.y + Sqr.r0 * Math.sin(fi);
	fi += pipol;
	verts[1].x = Sqr.x + Sqr.r0 * Math.cos(fi);
	verts[1].y = Sqr.y + Sqr.r0 * Math.sin(fi);
	fi += pipol;
	verts[2].x = Sqr.x + Sqr.r0 * Math.cos(fi);
	verts[2].y = Sqr.y + Sqr.r0 * Math.sin(fi);
	fi += pipol;
	verts[3].x = Sqr.x + Sqr.r0 * Math.cos(fi);
	verts[3].y = Sqr.y + Sqr.r0 * Math.sin(fi);
	return verts;
}

function circleRadius(){return this.r0};

function square(SQ){//{x:x0,y:y0,bok:bok0,ang:ang0,color:color}
	function radius(fi){return sqr(this.r0,fi-this.ang);};
	function draw(ctx, style, color){
		if(color==undefined){
			color = this.color;
		}
		drawSquare(ctx, this, style, color);
	};
	return{x:SQ.x,
			 y:SQ.y,
			 bok:SQ.bok,
			 r0:SQ.bok*sqrt2przez2,
			 ang:norm(SQ.ang),
			 draw:draw,
			 r:radius,
			 color:SQ.color,
			 type: 'Square'
		};
}

function circle(B){//{x:x,y:y,r0:r0,color:color}
	function draw(ctx, style, color){
		if(color==undefined){
			color = this.color;
		}
		drawCircle(ctx, this, style, color);
	};
	return{x:B.x,
	       y:B.y,
			 r0:B.r0,
			 r:circleRadius,
			 ang:0,
			 draw:draw,
			 color:B.color,
			 type: 'Circle'
	};
}

function rectangle(R){	//{a:a,b:b,x:x,y:y,ang:ang,color:color}
	R.a = abs(R.a);
	R.b = abs(R.b);
	var a=Math.max(R.a,R.b),
		 b=Math.min(R.a,R.b),	
		 fi0=Math.abs(2*Math.atan(b/a)),
		 ang = R.ang;
	if(R.b > R.a){
		ang += pipol;
	}
	function wsp(fi){
							fi=norm(fi);
							if( ((fi>=pipol)&&(fi<=pipol+this.fi0)) || ((fi>=pi32)&&(fi<=pi32+this.fi0)) )
								return this.wsp2;
							return this.wsp1;
						};
	function r(fi){
							fi -= - this.fi0 / 2 - this.ang; //ew   + pi
							var res=Math.sin(fi) - this.wsp(fi) * Math.cos(fi);
							if(res)
								res=Math.abs(this.r0/res);							
							return res;
		};
	function draw(ctx, style, color){
		if(color==undefined){
			color = this.color;
		}
		drawRect(ctx, this, style, color);
	};
	return {
		a: a,
		b: b,
		x: R.x,
		y: R.y,
		fi0: fi0,
		r0: b / (Math.sin(fi0 / 2) * 2),
		ang: norm(ang),
		wsp1: Math.tan(pipol + fi0 / 2),
		wsp2: Math.tan(fi0 / 2),
		wsp: wsp,
		r: r,
		color: R.color,
		draw: draw,
		type: 'Rectangle'
		}
	}

function player(Pl){
	function draw(ctx, style, color){
		drawPlayer(ctx, this, style, color);
	}
	return {
		x: Pl.x,
		y: Pl.y,
		ang: Pl.ang,
		r: function(){return PlayerSize;},
		r0: PlayerSize,
		draw: draw,
		type: 'Player'
	}
}

function entrance(Ent){
	function draw(ctx, style, color){
		drawEntrance(ctx, this, style, color);
	}
	Ent = rectangle(Ent);
	Ent.type = 'Entrance';
	Ent.draw = draw;
	return Ent;
}

function exit(Ext){
	function draw(ctx, style, color){
		drawExit(ctx, this, style, color);
	}
	Ext = rectangle(Ext);
	Ext.type = 'Exit';
	Ext.draw = draw;
	return Ext;
}

function aswitch(S){	//{a:a,b:b,x:x,y:y,ang:ang,color:color,swType:switchtype,pos:pos,connected:{Doors:Array(), Switches:Array()}, mass:mass}
	function draw(ctx, style, color){
		drawSwitch(ctx, this, style, color);
	}
	var Sw = rectangle(S);
	Sw.type = 'Switch';
	Sw.draw = draw;
	Sw.swType = S.swType;
	Sw.pos = S.pos;
	if(Sw.pos < 0){
		Sw.pos = 0;
	}else if(Sw.pos > MaxPos){
		Sw.pos = MaxPos;
	}
	Sw.connected = S.connected;
	Sw.mass = S.mass;
	return Sw;
}

function door(D){ //{a:a,b:b,x:x,y:y,ang:ang,color:color,doorType:doorType,pos:pos,openingTime:openingTime}
	function draw(ctx, style, color){
		drawDoor(ctx, this, style, color);
	}
	var Dr = rectangle(D);
	Dr.type = 'Door';
	Dr.draw = draw;
	Dr.doorType = D.doorType;
	Dr.pos = D.pos;
	if(Dr.pos < 0){
		Dr.pos = 0;
	}else if(Dr.pos > MaxPos){
		Dr.pos = MaxPos;
	}
	Dr.openingTime = D.openingTime
	return Dr;
}

function particle(P){//{x:x,y:y,r0:r0,charge:charge,mass:mass}
	function draw(ctx, style, color){
		if(color==undefined){
			color = this.color;
		}
		drawParticle(ctx, this, style, color);
	}
	var c = P.charge / MaxCharge;
	if(c > 1){
		c = 1;
	}else if(c < -1){
		c = -1;
	}
	
	if(c === 0){
		c = "#fff";
	}else{
		var val = 255 - abs(Math.floor(c * 255));
		if(c > 0){
			c = "rgb(" + 255 + "," + val + "," + val + ")";
		}else{
			c = "rgb("+ val + "," + val + "," + 255 + ")";
		}
	}
	return {
		type: 'Particle',
		x: P.x,
		y: P.y,
		r0: P.r0,
		r: circleRadius,
		ang: P.ang,
		charge: P.charge,
		draw: draw,
		color: c,
		mass: P.mass
	}
}

function centroid(arg){
	return {	x: arg.x
					,	y: arg.y
					,	draw: function(ctx, style, color){
							drawCentroid(ctx, this, style);
						}
					};
}
//-------------------------------------------------------------

function drawStyleAspect(ctx, style, color){
	ctx[style + 'Style'] = color;
	ctx[style]();
}

function drawSquareSTUPID(ctx, sq, style, color){
	var r, x, y;
	x = sq.x + sq.r(0);
	y = sq.y;
	ctx.moveTo(x,y);
	ctx.beginPath();
	for(var i = jedna50stopnia; i < dwapi + jedna50stopnia; i += jedna50stopnia){
		r=sq.r(i);
		x=sq.x + r * Math.cos(i);
		y=sq.y + r * Math.sin(i);
		ctx.lineTo(x,y);
	}
	ctx.closePath();
	drawStyleAspect(ctx, style, color);
}

function drawSquare(ctx, sq, style, color){
	var v = findSquareVertices(sq);
	ctx.moveTo(v[0].x, v[0].y);
	ctx.beginPath();
	ctx.lineTo(v[1].x, v[1].y);
	ctx.lineTo(v[2].x, v[2].y);
	ctx.lineTo(v[3].x, v[3].y);
	ctx.lineTo(v[0].x, v[0].y);
	ctx.closePath();
	drawStyleAspect(ctx, style, color);
}

function drawRectSTUPID(ctx, rc, style, color){
	var r,x,y;
	r=rc.r(0);
	x=rc.x+r; //fi == 0
	y=rc.y;
	ctx.moveTo(x,y);
	ctx.beginPath();
	for(var i=jedna50stopnia; i<=dwapi+jedna50stopnia; i+=jedna50stopnia){
		r=rc.r(i);
		x=rc.x+r*Math.cos(i);
		y=rc.y+r*Math.sin(i);
		ctx.lineTo(x,y);
	}
	drawStyleAspect(ctx, style, color);
}

function drawRect(ctx, rc, style, color){
	var v = findRectVertices(rc);
	ctx.moveTo(v[0].x, v[0].y);
	ctx.beginPath();
	ctx.lineTo(v[1].x, v[1].y);
	ctx.lineTo(v[2].x, v[2].y);
	ctx.lineTo(v[3].x, v[3].y);
	ctx.lineTo(v[0].x, v[0].y);
	ctx.closePath();
	drawStyleAspect(ctx, style, color);
}

function drawCircle(ctx, bl, style, color){
	ctx.beginPath();
	ctx.arc(bl.x,bl.y,bl.r(),0,dwapi,false);
	ctx.closePath();
	drawStyleAspect(ctx, style, color);
}

function drawPlayer(ctx, Pl, style, color){
	if(color != undefined){
		ctx.beginPath();
		ctx.arc(Pl.x, Pl.y, PlayerSize, 0, dwapi, false);
		ctx.closePath();
		drawStyleAspect(ctx, style, color);
		ctx.moveTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		ctx.beginPath();
		ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang - pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang - pi34));
		ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang + pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang + pi34));
		ctx.lineTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		ctx.closePath();
		drawStyleAspect(ctx, style, "#000");
	}
	else{
		ctx.beginPath();
		ctx.arc(Pl.x,Pl.y,PlayerSize,0,dwapi,false);
		ctx.closePath();
		drawStyleAspect(ctx, style, "#000");
		ctx.beginPath();
		ctx.arc(Pl.x,Pl.y,PlayerSize * 0.66,0,dwapi,false);
		ctx.closePath();
		drawStyleAspect(ctx, style, "#fff");
		ctx.beginPath();
		ctx.arc(Pl.x,Pl.y,PlayerSize * 0.33,0,dwapi,false);
		ctx.closePath();
		drawStyleAspect(ctx, style, "#000");
		ctx.moveTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		ctx.beginPath();
		ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang - pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang - pi34));
		ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang + pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang + pi34));
		ctx.lineTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		ctx.closePath();
		drawStyleAspect(ctx, style, "#000");
	}
}

function drawEntrance(ctx, Ent, style, color){
	var temp;
	if(color === undefined){
		color = Ent.color;
	}
	drawRect(ctx, Ent, style, color);
	temp = Ent.r0;
	Ent.r0 *= 0.66;
	drawRect(ctx, Ent, style, '#000');
	Ent.r0 = temp;
	ctx.fillStyle = "#f00";
	ctx.fillText('IN', Ent.x - 10, Ent.y + 10);
}

function drawExit(ctx, Ext, style, color){
	var temp;
	if(color === undefined){
		color = Ext.color;
	}
	drawRect(ctx, Ext, style, color);
	temp = Ext.r0;
	Ext.r0 *= 0.66;
	drawRect(ctx, Ext, style, '#000');
	Ext.r0 = temp;
	ctx.fillStyle = "#f00";
	ctx.fillText('OUT', Ext.x - 10, Ext.y + 10);
}

function drawSwitch(ctx, Sw, style, color){
	if(color === undefined){
		color = Sw.color;
	}
	switch(Sw.swType){
		case 'slide':
			var Op = rectangle(Sw),
				 temp,
				 ang;
			Op.b *= 0.1;
			Op = rectangle(Op);
			
			temp = Sw.b * 0.45;

			Op.x -= temp * Math.cos(-Sw.ang);
			Op.y -= temp * Math.sin(-Sw.ang);
			drawRect(ctx, Op, style, color);
			
			
			Op.x += 2 * temp * Math.cos(-Sw.ang);
			Op.y += 2 * temp * Math.sin(-Sw.ang);
			drawRect(ctx, Op, style, color);
			
			Op = rectangle(Sw);
			Op.a *= 0.1;
			Op.b *= 0.8;
			Op = rectangle(Op);
			
			temp = Sw.a * 0.45;
			ang = pipol - Sw.ang;
			
			Op.x -= temp * Math.cos(ang);
			Op.y -= temp * Math.sin(ang);
			drawRect(ctx, Op, style, "#f00");
				
			Op.x += 2 * temp * Math.cos(ang);
			Op.y += 2 * temp * Math.sin(ang);
			drawRect(ctx, Op, style, "#0f0");
			
			temp = Sw.a * 0.7 * Sw.pos / MaxPos - Sw.a * 0.35;
			Op.x = Sw.x + temp * Math.cos(ang);
			Op.y = Sw.y + temp * Math.sin(ang);
			drawRect(ctx, Op, style, color);
			break;
		default:
			drawRect(ctx, Sw, style, color);
	}
}

function drawDoor(ctx, Dr, style, color){
	if(color === undefined){
		color = Dr.color;
	}
	switch(Dr.doorType){
		case '1wingSlide':
			var Op = rectangle(Dr),
				 temp,
				 ang;
			Op.b *= 0.8;
			temp = Dr.pos / MaxPos * Op.a;
			ang = norm(pipol - Op.ang);
			Op.x -= temp * Math.cos(ang);
			Op.y -= temp * Math.sin(ang);
			Op = rectangle(Op);
			drawRect(ctx, Op, style, color);
			
			Op = rectangle(Dr);
			temp = Dr.b * 0.45;
			Op.b *= 0.1;
			Op.x -= temp * Math.cos(-Op.ang);
			Op.y -= temp * Math.sin(-Op.ang);
			Op = rectangle(Op);
			drawRect(ctx, Op, style, color);
			
			Op.x += 2 * temp * Math.cos(-Op.ang);
			Op.y += 2 * temp * Math.sin(-Op.ang);
			drawRect(ctx, Op, style, color);
			break;
		default:
			drawRect(ctx, Dr, style, color);
	}
	
}

function drawParticle(ctx, P, style, color){
	drawCircle(ctx, P, style, color);
	ctx.fillStyle = "#000";
	
	if(P.charge === 0){
		color = 'n';
	}else if(P.charge > 0){
		color = '+';
	}else if(P.charge < 0){
		color = '-';
	}
	ctx.fillText(color, P.x - 8, P.y + 8);
}

function drawPoint(ctx, point, style, color){
	ctx.beginPath();
	ctx.arc(point.x, point.y, 3, 0, dwapi, false);
	ctx.closePath();
	drawStyleAspect(ctx, style, color);

	ctx.beginPath();
	ctx.arc(point.x, point.y, 2, 0, dwapi, false);
	ctx.closePath();
	drawStyleAspect(ctx, style, color);
}

function drawCentroid(ctx, Cent, style){
	drawPoint(ctx, Cent, style, 'rgba(255, 0, 0, 0.9)');
}

function drawModuleCentroid(Module, style){
	if(Module.selectedCentroid){
		Module.selectedCentroid.draw(Module.$secondCtx, style);
	}
}

function drawTransformedSelection(Module, style, color){
	var x
		,	y;
	function drawObj(obj){
		obj.draw(Module.$secondCtx, style, color);
	}
	
	clearSecondCanva(Module);
	if(Module.Selected.length > 0){
		transformSelection(Module, Module.transArgs, drawObj);
		drawModuleCentroid(Module, 'fill');
	}
}

function transformSelection(Module, args, func){
	if(Module.selectedCentroid){
		Module.Selected.push(Module.selectedCentroid);
	}
	if(args.type == null){
		if(Module.rotate){
			args.type = 'turn';
		}else{
			args.type = 'move';
		}
	}
	switch(args.type){
		case 'turn':
			turnArray(Module.Selected, Module.rotationPoint, args.ang, func);
			break;
		case 'move':
			moveArray(Module.Selected, args.x, args.y, func);
			break;
	}
	
	if(Module.selectedCentroid){
		Module.Selected.pop();
	}
}

//-------------------------------------------------------------

function mysz(e) {
	if (!e) e = window.event;
	$disp.textContent='x= ' + e.pageX + ' y= ' + e.pageY;
}

function chooseObjType(type){
	$('#objType' + checkedOne)[0].checked = false;
	checkedOne = type;
	$('#objType' + checkedOne)[0].checked = true;
}

function isClicked(Obj,e){
	var dx,
		 dy,
		 ang;
	dx = e.pageX - Obj.x;
	dy = e.pageY - Obj.y;
	ang = VectorAtan(dx, dy);
	dx = Math.sqrt(dx * dx + dy * dy);
	if(dx <= Obj.r(ang))
		return true;
	return false;
}

function selectObjOnCanvas(e, Module){//mousedown
	if (!e) e = window.event;
	var relTarg = e.relatedTarget || e.fromElement
		,	Obj = false
		,	j
		,	i
		,	len
		,	len2;
	
	Module.mouseDragX = e.pageX;
	Module.mouseDragY = e.pageY;
	Module.isClicked = true;
	if(e.pageX < $canva.width &&
		 e.pageY < $canva.height){
		Module.lastMouseEvent = {x: e.pageX, y: e.pageY};
		if(Module.selectedCentroid){
			Module.computeMouseDownArgs(e.pageX, e.pageY);
			Module.transArgs = {x: 0, y: 0, ang: 0};
		}
		//finding if any object was clicked
		len = Module.Arrays.length;
		for(i = 0; i < len; ++i){
			if(!Obj){
				len2 = Module[Module.Arrays[i]].length;
				for(j = 0; j < len2; ++j){
					if(isClicked(Module[Module.Arrays[i]][j],e)){
						Obj = Module[Module.Arrays[i]][j];
						break;
					}
				}
			}
			else{
				break;
			}
		}
		
		if(!Obj){
			Module.Clicked = {};
		}else{
			Module.Clicked = Obj;
		}
		
		if(Module.Selected.length === 0){
			Module.IsDragging = false;
		}else{
			if(!Obj){
				Module.IsDragging = false;
			}else{
				Module.IsDragging = false;
				len = Module.Selected.length;
				for(i = 0; i < len; ++i){
					if(Module.Selected[i] === Obj){
						Module.IsDragging = true;
						break;
					}
				}
			}
		}
	}
}

function dropOnCanvas(e, Module){//mouseup
	var i
		,	j
		,	ang
		,	x
		,	y
		,	SelectionRect
		,	len
		,	len2;
	
	if (!e) e = window.event;
	Module.isClicked = false;
	Module.$selectRect.width = 0;
	Module.$selectRect.height = 0;
	if(Module.mouseDragX < $canva.width &&
		 Module.mouseDragY < $canva.height){
		clearSecondCanva(Module);
		if(e.PageX > $canva.width){
			e.PageX = $canva.width;
		}
		if(e.PageY > $canva.height){
			e.PageY = $canva.height;
		}
		if(Module.IsDragging ||
			(Module.rotate && Module.selectedCentroid)){
			Module.IsDragging = false;
			Module.computeTransArgs(e.pageX, e.pageY);
			if(e.shiftKey){//clone
				cloneObject(Module);
			}else{//move
				transformSelection(Module, Module.transArgs);
				ReDraw(Module);
			}
			ShowObj(Module.Selected);
		}else{//selection
			x = e.pageX - Module.mouseDragX;
			y = e.pageY - Module.mouseDragY;
			SelectionRect = rectangle({x: (e.pageX + Module.mouseDragX) / 2,
													 y: (e.pageY + Module.mouseDragY) / 2,
													 a: y,
													 b: x,
													 ang: 0,
													 color: SelectionColor
													});
			Module.LastSelected = Module.Selected;
			Module.Selected = [];
			
			len = Module.Arrays.length;
			for(i = 0; i < len; ++i){
				len2 = Module[Module.Arrays[i]].length;
				for(j = 0; j < len2; ++j){
					x = SelectionRect.x - Module[Module.Arrays[i]][j].x;
					y = SelectionRect.y - Module[Module.Arrays[i]][j].y;
					ang = VectorAtan(x, y);
					ang = Math.sqrt(x * x + y * y) - (SelectionRect.r(ang) + Module[Module.Arrays[i]][j].r(norm(ang + pi)));
					if(ang < 0){
						Module.Selected.push(Module[Module.Arrays[i]][j]);
					}
				}
			}
			
			if(Module.Clicked.x != undefined){
				j = true;
				len = Module.Selected.length;
				for(i = 0; i < len; ++i){
					if(Module.Selected[i] === Module.Clicked){
						j = false;
						break;
					}
				}
				if(j){
					Module.Selected.push(Module.Clicked);
				}
			}
			
			Module.selectedCentroid = findCentroid(Module.Selected);
			Module.setRotationPoint();

			ShowObj(Module.Selected);
			drawSelection();
		}
		Module.transArgs = {x: 0, y: 0, ang: 0};
		SetSelectedOnListFromData();
	}

}

function clearSecondCanva(Module){
	Module.$secondCtx.clearRect(0, 0,
		Module.$secondCanva.width - 1,
		Module.$secondCanva.height - 1);
}

function clearThirdCanva(Module){
	Module.$thirdCtx.clearRect(0, 0,
		Module.$thirdCanva.width - 1,
		Module.$thirdCanva.height - 1);
}

function drawUserRotationPoint(Module){
	clearThirdCanva(Module);
	drawPoint(Module.$thirdCtx, Module.userRotPoint, 'fill', 'rgba(0, 255, 0, 0.6)');
}

function drawAtMouseMove(e, Module){
	var i
		,	len
		,	evX
		,	evY
		,	dx
		,	dy
		,	color;
	if (!e) e = window.event;
	
	mysz(e);
	
	if(Module.mouseDragX < $canva.width &&
		 Module.mouseDragY < $canva.height &&
		 Module.isClicked){

		if(e.pageX > $canva.width){
			evX = $canva.width;
		}else{
			evX = e.pageX;
		}
		if(e.pageY > $canva.height){
			evY = $canva.height;
		}else{
			evY = e.pageY;
		}
		Module.lastMouseEvent = {x: evX, y: evY};

		if(e.shiftKey){
			color = 'rgba(0, 255, 0, 0.9)';
		}else{
			color = SelectionColor;
		}
		if(Module.IsDragging ||
			(Module.rotate && Module.selectedCentroid)){
			Module.computeTransArgs(evX, evY);
			drawTransformedSelection(Module, 'stroke', color);
		}else if(Module.isClicked){
			drawSelectRect(Module);
		}
	}
}

function drawSelectRect(Module){
	var width
		,	height;

	Module.$selectRect.style.left = min(Module.mouseDragX, Module.lastMouseEvent.x) + "px";
	Module.$selectRect.style.top = min(Module.mouseDragY, Module.lastMouseEvent.y) + "px";
	width = abs(Module.lastMouseEvent.x - Module.mouseDragX);
	height = abs(Module.lastMouseEvent.y - Module.mouseDragY);
	Module.$selectRect.width = width;
	Module.$selectRect.height = height;
	Module.$selectRect.style.backgroundColor = selectRectColor;

	Module.$selectCtx.clearRect(0, 0, width - 1, height - 1);
}

//-------------------------------------------------------------

function sortfunc(a, b){
	return a - b;
}

function d2h(d){
	return d.toString(16);
}

function h2d(h){
	return parseInt(h,16);
}

function Shorth2d(h){
	h = h2d(h);
	return h * 16 + h;
}

//-------------------------------------------------------------

function abs(a){
	if(a>0)
		return a;
	return -a;
}

function min(a, b){
	return a < b ? a : b;
}

function sign(a){
	if(!a)
		return 0;
	if(a>0)
		return 1;
	return -1;
}

function norm(fi){
	if(fi > 0){
		while(fi > dwapi){
			fi -= dwapi;
		}
	}else
		while(fi < 0){
			fi += dwapi;
		}
	return fi;
}

function VectorAtan(x,y){
   if(x != 0){
		var b;
		b = Math.atan(abs(y / x));
		if(y === 0){
			if(x < 0){
				b = pi;
			}
		}
		else if(x < 0){
			if(y > 0){
				b = pi - b;
			}else{
				b += pi;
			}
		}
		else if(y < 0){
			b = dwapi - b;
		}
		return b;
    }
    else{
		if(y === 0)
			return 0;
		if(y > 0)
			return pi / 2;
		return pi32;
    }
}

function rotateObj(Obj, ang){
	if(Obj.ang != undefined){
		if(Obj.a != undefined){//rectangle
			Obj.ang -= ang;
		}else{
			Obj.ang += ang;
		}
	}
}

// Simplest, no ifs
function cachedTurn(point, dx, dy, ang, sn, cs){
	rotateObj(point, ang);
	point.x -= dx;
	point.y -= dy;

	ang = point.x * cs - point.y * sn;
	point.y = point.x * sn + point.y * cs;
	point.x = ang;
	
	point.x += dx;
	point.y += dy;
}

//Simple wrapper for one object
function turnObj(point, p0, ang){
	cachedTurn(point, p0.x, p0.y, ang, Math.sin(ang), Math.cos(ang));
}

// Allows turning objects around point  p0.
// If func parameter is passed then objects
// will be turned then func will be exectuted
// and the array will return to status quo.
function turnArray(arr, p0, ang, func){
	var i
		,	len
		,	dx
		,	dy
		,	sn
		,	cs
		,	turnType
		,	oldX
		,	oldY
		,	oldAng;
	if(ang){
		if(p0 === 'indie'){
			turnType = function(obj, dx, dy, ang, sn, cs){
				rotateObj(obj, ang);
			}
		}else{
			sn = Math.sin(ang);
			cs = Math.cos(ang);
			dx = p0.x;
			dy = p0.y;
			turnType = cachedTurn;
		}
		
		len = arr.length;
		if(func){
			for(i = 0; i < len; ++i){
				oldX = arr[i].x;
				oldY = arr[i].y;
				oldAng = arr[i].ang;
				turnType(arr[i], dx, dy, ang, sn, cs);
				func(arr[i]);
				arr[i].x = oldX;
				arr[i].y = oldY;
				arr[i].ang = oldAng;
			}
		}else{
			for(i = 0; i < len; ++i){
				turnType(arr[i], dx, dy, ang, sn, cs);
			}
		}
	}
}

function movePoint(point, dx, dy){
	point.x += dx;
	point.y += dy;
}

//Another transformation with the same logical structure
function moveArray(arr, dx, dy, func){
	var i
		,	len
		,	oldX
		,	oldY;
	len = arr.length;
	if(func){
		for(i = 0; i < len; ++i){
			oldX = arr[i].x;
			oldY = arr[i].y;
			movePoint(arr[i], dx, dy);
			func(arr[i]);
			arr[i].x = oldX;
			arr[i].y = oldY;
		}
	}else{
		for(i = 0; i < len; ++i){
			movePoint(arr[i], dx, dy);
		}
	}
}
//var b; for(var i = -100; i < 100; i+= 0.1){ for(var j = -100; j < 100; j+= 0.1){ b = VectorAtan(i, j); if((b > dwapi) || (b < 0)) console.log([i, j, b]);}}
//no logs, no need to normalize returned angle

function vectorProduct(v1, v2){
	return v1.x * v2.y - v2.x * v1.y;
}

// for vectors v1->v2, v2->v3 returns
// negative value if going from first
// to second vector means turning left
// and positive otherwise
function turnSide(v1, v2, v3){
	return vectorProduct({x: v3.x - v1.x,
												y: v3.y - v1.y},
											 {x: v2.x - v1.x,
												y: v2.y - v1.y});
}

function compPointsByY(A, B){
	if(A.y === B.y){
		return A.x - B.x;
	}else{
		return A.y - B.y;
	}
}

function compPointsByAng(A, B){
	if(A.ang === B.ang){
		return compPointsByY(B, A);
	}else{
		return A.ang - B.ang;
	}
}

function convexHull(points){
	return convexHullGraham(points);
}

function convexHullGraham(argPoints){
	var p0
		,	i
		,	len
		,	pi
		,	stack
		, points = [];
	len = argPoints.length;
	
	if(len < 3){
		stack = [];
	}else{
		p0 = 0;
		for(i = 0; i < len; ++i){
			points[i] = {x: argPoints[i].x, y: argPoints[i].y};
			if(compPointsByY(points[p0], points[i]) > 0){
				p0 = i;
			}
		}
		p0 = points.extract(p0);
		
		len = points.length;
		for(i = 0; i < len; ++i){
			pi = points[i];
			pi.ang = VectorAtan(pi.x - p0.x, pi.y - p0.y);
		}
		points.sort(compPointsByAng);
		for(i = 0; i < len; ++i){
			delete points[i].ang;
		}
		
		stack = [p0, points[0], points[1]];
		len = points.length;
		for(i = 2; i < len; ++i){
			while(turnSide(stack[stack.length - 2], stack[stack.length - 1], points[i]) > 0){
				stack.pop();
			}
			stack.push(points[i]);
		}
	}
	return stack;
}

function findCentroid(points){
	var convHull
		,	center;

	if(points.length !== 0){
		convHull = convexHull(points);
		if(convHull.length === 0){
			convHull = points;
		}

		center = convHull.reduce(function(pre, cur, i, arr){
			return {x: cur.x + pre.x, y: cur.y + pre.y};
		}, {x: 0, y: 0});
		center.x /= convHull.length;
		center.y /= convHull.length;
		return centroid(center);
	}else{
		return null;
	}
}