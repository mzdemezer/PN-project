//TODO:
/*
- writing second version of drawing functions that would find verticies
- some new objects? I have many more ideas, these are only very basic ones, so I will be able to finish on time xD
*/

// I know drawing is HIGHLY non-optimal, but this is about maths and equations in polar coordinate system
// Drawing is not going to work that way in game, no :P
// Also, I know the number of globals is outrageous, but I'm in a hurry

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
	 $output,
	 $sieczka;
const wdth=750,
		hght=750,
		SelectionColor="#66f",
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
					Squares:Array(),
					Rectangles:Array(),
					Circles:Array(),
					Players:Array(),
					Entrances:Array(),
					Exits:Array(),
					Switches:Array(),
					Doors:Array(),
					Particles:Array(),
					Add: AddObjectToData,
					Selected:[],
					LastSelected:[],
					Clicked:{},
					IsDragging: false,
					Background: {Path: "default", Img: new Image()},
					mouseDragX:0,
					mouseDragY:0
			};

if (document.addEventListener) {
  document.addEventListener('DOMContentLoaded', init, false);
}

function init(){
	if (arguments.callee.done) return;
	arguments.callee.done = true;
	
	document.onmousemove = mysz;
	//document.onkeypress = klawa;
	document.onkeyup = klawa;
	
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
	
	$disp = gEBI('disp');
	$canva = gEBI('canva');
	$canva.width = wdth;
	$canva.height = hght;
	
	$canva.onmousedown = selectObjOnCanvas;
	$canva.onmouseup = dropOnCanvas;
	$canva.onmouseout = function(){
								Data.IsDragging = false;
							};
	
	$ctx = $canva.getContext('2d');
	$ImgD = $ctx.getImageData(0,0,wdth-1,hght-1);
	
	$ctx.lineWidth = 1.6;
	$ctx.font = '25px Verdana';
	
	ObjRadioList = Array();
	ObjRadioList['Circle'] = gEBI('objTypeCirlce');
	ObjRadioList['Square'] = gEBI('objTypeSquare');
	ObjRadioList['Rectangle'] = gEBI('objTypeRectangle');
	ObjRadioList['Player'] = gEBI('objTypePlayer');
	ObjRadioList['Entrance'] = gEBI('objTypeEntrance');
	ObjRadioList['Exit'] = gEBI('objTypeExit');
	ObjRadioList['Switch'] = gEBI('objTypeSwitch');
	ObjRadioList['Door'] = gEBI('objTypeDoor');
	ObjRadioList['Particle'] = gEBI('objTypeParticle');
	
	$output = gEBI('output');
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
		
		CallDrawing = true;
		ListOut();
		SetSelectedOnListFromData();
		ShowObj(Data.Selected);
	//...up to here
	
	ReDraw();
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

function GenerateOutput(){
	var op, op2, op3;
	$output.value = Data.Background.Path + "\n"; 
	if(Data.Background.Path === "default"){
		$sieczka.value = "0\n";
	}else{
		$sieczka.value = Data.Background.Path + "\n";
	}
	
	$output.value += "Players " + Data.Players.length + "\n";
	$sieczka.value += Data.Players.length + "\n";
	for(var i in Data.Players){
		op = roundToTwo(Data.Players[i].x) + " " + 
			  roundToTwo(Data.Players[i].y) + " " + 
			  roundToTwo(Data.Players[i].ang) +
			  "\n";
		$output.value += op;
		$sieczka.value += op;
	}
	
	$output.value += "Rectangles " + Data.Rectangles.length + "\n";
	$sieczka.value += Data.Rectangles.length + "\n";
	for(var i in Data.Rectangles){
		op = roundToTwo(Data.Rectangles[i].x) + " " + 
			  roundToTwo(Data.Rectangles[i].y) + " " + 
			  roundToTwo(Data.Rectangles[i].a) + " " + 
			  roundToTwo(Data.Rectangles[i].b) + " " + 
			  roundToTwo(Data.Rectangles[i].ang) + " ";
		op2 = op;
		op += Data.Rectangles[i].color + 
			  "\n";
		op2 += ColorToNumbers(Data.Rectangles[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}
	
	$output.value += "Circles " + Data.Circles.length + "\n";
	$sieczka.value += Data.Circles.length + "\n";
	for(var i in Data.Circles){
		op = roundToTwo(Data.Circles[i].x) + " " + 
			  roundToTwo(Data.Circles[i].y) + " " +
			  roundToTwo(Data.Circles[i].r0) + " ";
		op2 = op;
		op += Data.Circles[i].color + 
			  "\n";
		op2 += ColorToNumbers(Data.Circles[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}
	
	$output.value += "Squares " + Data.Squares.length + "\n";
	$sieczka.value += Data.Squares.length + "\n";
	for(var i in Data.Squares){
		op = roundToTwo(Data.Squares[i].x) + " " +
			  roundToTwo(Data.Squares[i].y) + " " +
			  roundToTwo(Data.Squares[i].bok) + " " +
			  roundToTwo(Data.Squares[i].ang) + " ";
		op2 = op;
		op += Data.Squares[i].color + 
			  "\n";
		op2 += ColorToNumbers(Data.Squares[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}
	
	$output.value += "Entrances " + Data.Entrances.length + "\n";
	$sieczka.value += Data.Entrances.length + "\n";
	for(var i in Data.Entrances){
		op = roundToTwo(Data.Entrances[i].x) + " " + 
			  roundToTwo(Data.Entrances[i].y) + " " + 
			  roundToTwo(Data.Entrances[i].a) + " " + 
			  roundToTwo(Data.Entrances[i].b) + " " + 
			  roundToTwo(Data.Entrances[i].ang) + " ";
		op2 = op;
		op += Data.Entrances[i].color + 
			  "\n";
		op2 += ColorToNumbers(Data.Entrances[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}
	
	$output.value += "Exits " + Data.Exits.length + "\n";
	$sieczka.value += Data.Exits.length + "\n";
	for(var i in Data.Exits){
		op = roundToTwo(Data.Exits[i].x) + " " + 
			  roundToTwo(Data.Exits[i].y) + " " + 
			  roundToTwo(Data.Exits[i].a) + " " + 
			  roundToTwo(Data.Exits[i].b) + " " + 
			  roundToTwo(Data.Exits[i].ang) + " ";
		op2 = op;
		op += Data.Exits[i].color + 
			  "\n";
		op2 += ColorToNumbers(Data.Exits[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}

	$output.value += "Switches " + Data.Switches.length + "\n";
	$sieczka.value += Data.Switches.length + "\n";
	for(var i in Data.Switches){
		op = roundToTwo(Data.Switches[i].x) + " " + 
							  roundToTwo(Data.Switches[i].y) + " " + 
							  roundToTwo(Data.Switches[i].a) + " " + 
							  roundToTwo(Data.Switches[i].b) + " " + 
							  roundToTwo(Data.Switches[i].ang) + " ";
		op2 = op;
	   op += Data.Switches[i].swType + " ";
		op2 += swTypeToNumber(Data.Switches[i].swType) + " ";
		
		op3 = roundToTwo(Data.Switches[i].pos) + " " +
			   Data.Switches[i].connected.Doors.length;
		op += op3 + " ";
		op2 += op3 + "\n";
		op3 = "";
		
		Data.Switches[i].connected.Doors.sort(sortfunc);
		Data.Switches[i].connected.Switches.sort(sortfunc);
		for(var j in Data.Switches[i].connected.Doors){
			op3 += Data.Switches[i].connected.Doors[j] + " ";
		}
		if(op3 != ""){
			op += op3;
			op2 += op3.slice(0,-1) + "\n";
		}
		
		op3 = Data.Switches[i].connected.Switches.length;
		op += op3 + " ";
		op2 += op3 + "\n";
		
		op3 = "";
		for(var j in Data.Switches[i].connected.Switches){
			op3 += Data.Switches[i].connected.Switches[j] + " ";
		}
		if(op3 != ""){
			op += op3;
			op2 += op3.slice(0,-1) + "\n";
		}
		op3 = roundToTwo(Data.Switches[i].mass) + " ";
		
		op += op3 + Data.Switches[i].color + 
			  "\n";
		op2 += op3 + ColorToNumbers(Data.Switches[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}
	
	$output.value += "Doors " + Data.Doors.length + "\n";
	$sieczka.value += Data.Doors.length + "\n";
	for(var i in Data.Doors){
		op = roundToTwo(Data.Doors[i].x) + " " + 
			  roundToTwo(Data.Doors[i].y) + " " + 
			  roundToTwo(Data.Doors[i].a) + " " + 
			  roundToTwo(Data.Doors[i].b) + " " + 
			  roundToTwo(Data.Doors[i].ang) + " ";
		op2 = op;
		op += Data.Doors[i].doorType;
		op2 += doorTypeToNumber(Data.Doors[i].doorType);
		op3 = " " + roundToTwo(Data.Doors[i].pos) + " " +
			   roundToTwo(Data.Doors[i].openingTime) + " ";
		
		op += op3 + Data.Doors[i].color + 
			  "\n";
		op2 += op3 + ColorToNumbers(Data.Doors[i].color) +
			  "\n";
		$output.value += op;
		$sieczka.value += op2;
	}
	
	$output.value += "Particles " + Data.Particles.length + "\n";
	$sieczka.value += Data.Particles.length + "\n";
	for(var i in Data.Particles){
		op = roundToTwo(Data.Particles[i].x) + " " + 
			  roundToTwo(Data.Particles[i].y) + " " + 
			  roundToTwo(Data.Particles[i].r0) + " " + 
			  roundToTwo(Data.Particles[i].mass) + " " + 
			  roundToTwo(Data.Particles[i].charge) +
			  "\n";
		$output.value += op;
		$sieczka.value += op;
	}
	$output.value += $output.value.slice(0, -1);
	$sieczka.value = $sieczka.value.slice(0, -1);
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

function Load(){
	var input = $output.value.split('\n'),
		 i,
		 j,
		 drawing = CallDrawing;
	
	CallDrawing = false;
	
	for(i in input){
		input[i] = input[i].split(' ');
	}
	for(i in Data.Arrays){
		Data[Data.Arrays[i]] = [];
	}
	Data.Selected = [];
	Data.LastSelected = [];
	
	SetBackground(input[0][0]);
	
	for(i = 1; i < input.length; i += (input[i][1] - 0) + 1){
		switch(input[i][0]){
				case "Players":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(player({x: input[j][0] - 0, y: input[j][1] - 0, ang: input[j][2] - 0}));
					}
					break;
				case "Rectangles":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(rectangle({x: input[j][0] - 0, y: input[j][1] - 0, a: input[j][2] - 0, b: input[j][3] - 0, ang: input[j][4] - 0, color: input[j][5]}));
					}
					break;
				case "Circles":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(circle({x: input[j][0] - 0, y: input[j][1] - 0, r0: input[j][2] - 0, color: input[j][3]}));
					}
					break;
				case "Squares":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(square({x: input[j][0] - 0, y: input[j][1] - 0, bok: input[j][2] - 0, ang: input[j][3] - 0, color: input[j][4]}));
					}
					break;
				case "Entrances":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(entrance({x: input[j][0] - 0, y: input[j][1] - 0, a: input[j][2] - 0, b: input[j][3] - 0, ang: input[j][4] - 0, color: input[j][5]}));
					}
					break;
				case "Exits":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(exit({x: input[j][0] - 0, y: input[j][1] - 0, a: input[j][2] - 0, b: input[j][3] - 0, ang: input[j][4] - 0, color: input[j][5]}));
					}
					break;
				case "Switches":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						var connected = {	Doors: [],
												Switches: []
											 };
						for(var k = 8; k < (input[j][7] - 0) + 8; k+=1){
							connected.Doors.push(input[j][k] - 0);
						}
						var swlngthPos = k;
						for(k+=1; k <= (input[j][swlngthPos] - 0) + swlngthPos; k+=1){
							connected.Switches.push(input[j][k] - 0);
						}
						connected.Doors.sort(sortfunc);
						connected.Switches.sort(sortfunc);
						Data.Add(aswitch({x: input[j][0] - 0, y: input[j][1] - 0, a: input[j][2] - 0, b: input[j][3] - 0, ang: input[j][4] - 0, connected: connected, swType: input[j][5], pos: input[j][6] - 0, mass: input[j][k] - 0, color: input[j][k + 1]}));
					}
					break;
				case "Doors":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(door({x: input[j][0] - 0, y: input[j][1] - 0, a: input[j][2] - 0, b: input[j][3] - 0, ang: input[j][4] - 0, doorType: input[j][5], pos: input[j][6] - 0, openingTime: input[j][7] - 0, color: input[j][8]}));
					}
					break;
				case "Particles":
					for(j = i + 1; j <= i + (input[i][1] - 0); j+=1){
						Data.Add(particle({x: input[j][0] - 0, y: input[j][1] - 0, r0: input[j][2] - 0, mass: input[j][3] - 0, charge: input[j][4] - 0}));
					}
					break;
		}
	}
	
	ListOut();
	SetSelectedOnListFromData();
	ShowObj(Data.Selected);
	CallDrawing = drawing;
	ReDraw();
}

function Clear(){
	for(var i in Data.Arrays){
		Data[Data.Arrays[i]] = [];
	}
	for(i in $OptGroups){
		$OptGroups[i].empty();
	}
	Data.Selected = [];
	Data.LastSelected = [];
	ShowObj(Data.Selected);
	ReDraw();
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
		
		Data.Selected = [];
		
		if(CallDrawing){
			ReDraw();
		}
		
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
		
		Data.Selected = [];
		if(CallDrawing){
			ReDraw();
		}
		
	}
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
	ShowObj(Data.Selected);
	SetSelectedOnListFromData();
	if(CallDrawing){
		ReDraw();
	}
}

function AddToList(Obj, index){
	var GroupName;
	
	GroupName = "$Opt" + ObjNameToGroupName(Obj.type);
	$("<option></option>").text(GetSelectListNameFromObj(Obj, index)).appendTo($OptGroups[GroupName]);
}

function ListOut(){
	$ObjListSelect.detach();
	
	var i,j, objName, groupName;
	
	for(i in Data.Arrays){
		objName = GroupToObjName(Data.Arrays[i]);
		
		groupName = "$Opt" + Data.Arrays[i];
		$OptGroups[groupName].empty();
		
		for(j in Data[Data.Arrays[i]]){
			$("<option></option>").text(objName + " " + j).appendTo($OptGroups[groupName]);
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

function cloneObject(dx, dy){
	var newObj,
		 drawing = CallDrawing;
	
	for(var i in Data.Selected){
		Data.Selected[i].draw();
	}
	Data.LastSelected = Data.Selected;
	Data.Selected = [];
	CallDrawing = false;
	for(i in Data.LastSelected){
		if(Data.LastSelected[i]){
			switch(Data.LastSelected[i].type){
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
			newObj = newObj(Data.LastSelected[i]);
			newObj.x += dx;
			newObj.y += dy;
			AddToList(newObj, Data.Add(newObj));
			Data.Selected.push(newObj);
		}
	}
	ShowObj(Data.Selected);
	SetSelectedOnListFromData();
	CallDrawing = drawing;
	ReDraw();
}

//-------------------------------------------------------------

function SelectObject(){
	var Obj = GetObjectFromSelection(GetSelectedOnList());
	
	Data.LastSelected = Data.Selected;
	Data.Selected = [Obj];
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
	var i;
	for(i in Data.LastSelected){
		if(Data.LastSelected[i].draw){
			Data.LastSelected[i].draw();
		}
	}
	for(i in Data.Selected){
		if(Data.Selected[i].draw){
			Data.Selected[i].draw(SelectionColor);
		}
	}
}

function ShowObj(Objects){
	if(Objects.length > 0){
		var Obj = Objects[0];
		
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
		for(i in $ParamsInputList){
			$ParamsInputList[i].value = '';
			
		}
		$SpanObjType.textContent = 'Nothing';
	}
	//console.log(Obj);
}

//-------------------------------------------------------------

function Change(which){
	var NewValue = $ParamsInputList[which].value,
		 i;
	if(Data.Selected.length > 0){
		switch(which){
			case 'First':
				NewValue -= Data.Selected[0].x;
				for(i in Data.Selected){
					Data.Selected[i].x += NewValue;
					}
				break;
			case 'Second':
				NewValue -= Data.Selected[0].y;
				for(i in Data.Selected){
					Data.Selected[i].y += NewValue;
					}
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
						for(i in Data.Selected){
							if(Data.Selected[i].type === 'Particle'){
								Data.Selected[i].charge = NewValue;
								var j = GetObjectIndex(Data.Selected[i]);
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
				for(i in Data.Selected){
					if(Data.Selected[i].ang != undefined){
						Data.Selected[i].ang = NewValue;
					}
				}
				break;
			case 'Color':
				for(i in Data.Selected){
					if( (Data.Selected[i].type != "Player") && 
					 (Data.Selected[i].type != "Particle") ){
						Data.Selected[i].color = NewValue;
					}
				}
				
				break;
		}
		
		
		ShowObj(Data.Selected);
		if(CallDrawing){
			ReDraw();
		}
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

function ClearCanvas(){
	$ctx.fillStyle='rgba(255,128,0,255)';
	$ctx.fillRect(0,0,wdth-1,hght-1);
	$ctx.fillStyle='rgba(0,0,0,0.6)';
	$ctx.fillRect(0,0,wdth-1,hght-1);
	if(Data.Background.Path != "default"){
		$ctx.drawImage(Data.Background.Img,0,0);
	}
}

function ReDraw(){
	ClearCanvas();
	var i, j;
	for(i in Data.Arrays){
		for(j in Data[Data.Arrays[i]]){
			Data[Data.Arrays[i]][j].draw();
		}
	}
	for(i in Data.Selected){
		Data.Selected[i].draw(SelectionColor);
	}
}

function SetBackground(Path){
	Data.Background.Img.src = Path;
	if(Data.Background.Img.width > 0){
		Data.Background.Path = Path;
	}else{
		Data.Background.Path = "default";
	}
	$BackgroundText.value = Data.Background.Path
	if(CallDrawing){
		ReDraw();
	}
}

//-------------------------------------------------------------
function sqr(r0,fi){
	fi -= pi4;
	var sin=Math.sin(fi),
		 cos=Math.cos(fi);
	return abs(r0 / (sin + sign(sin*cos)*cos));
}

function circleRadius(){return this.r0};

function square(SQ){//{x:x0,y:y0,bok:bok0,ang:ang0,color:color}
	function radius(fi){return sqr(this.r0,fi-this.ang);};
	function draw(color){
		if(color==undefined){
			drawSquare(this,this.color);
		}
		else{
			drawSquare(this,color);
		}
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
	function draw(color){
		if(color==undefined){
			drawCircle(this,this.color);
		}
		else{
			drawCircle(this,color);
		}
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
							fi -= pi - this.fi0 / 2 - this.ang;
							var res=Math.sin(fi) - this.wsp(fi) * Math.cos(fi);
							if(res)
								res=Math.abs(this.r0/res);							
							return res;
		};
	function draw(color){
		if(color==undefined){
			drawRect(this,this.color);
		}
		else{
			drawRect(this,color);
		}
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
	function draw(color){
		drawPlayer(this, color);
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
	function draw(color){
		drawEntrance(this, color);
	}
	Ent = rectangle(Ent);
	Ent.type = 'Entrance';
	Ent.draw = draw;
	return Ent;
}

function exit(Ext){
	function draw(color){
		drawExit(this, color);
	}
	Ext = rectangle(Ext);
	Ext.type = 'Exit';
	Ext.draw = draw;
	return Ext;
}

function aswitch(S){	//{a:a,b:b,x:x,y:y,ang:ang,color:color,swType:switchtype,pos:pos,connected:{Doors:Array(), Switches:Array()}, mass:mass}
	function draw(color){
		drawSwitch(this, color);
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
	function draw(color){
		drawDoor(this, color);
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
	function draw(color){
		if(color != undefined){
			drawParticle(this, color);
		}
		else{
			drawParticle(this, this.color);
		}
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

//-------------------------------------------------------------

function drawSquare(sq, color){
	var r, x, y;
	x = sq.x + sq.r(0);
	y = sq.y;
	$ctx.moveTo(x,y);
	$ctx.beginPath();
	for(var i = jedna50stopnia; i < dwapi + jedna50stopnia; i += jedna50stopnia){
		r=sq.r(i);
		x=sq.x + r * Math.cos(i);
		y=sq.y + r * Math.sin(i);
		$ctx.lineTo(x,y);
	}
	$ctx.closePath();
	$ctx.fillStyle = color;
	$ctx.fill();
}

function drawRect(rc, color){
	var r,x,y;
	r=rc.r(0);
	x=rc.x+r; //fi == 0
	y=rc.y;
	$ctx.moveTo(x,y);
	$ctx.beginPath();
	for(var i=jedna50stopnia; i<=dwapi+jedna50stopnia; i+=jedna50stopnia){
		r=rc.r(i);
		x=rc.x+r*Math.cos(i);
		y=rc.y+r*Math.sin(i);
		$ctx.lineTo(x,y);
	}
	$ctx.fillStyle=color;
	$ctx.fill();
	$ctx.stroke();
	$ctx.closePath();
	r=rc.r(pipol + rc.ang + rc.fi0 / 2);
	$ctx.beginPath();
	$ctx.arc(rc.x+r*Math.cos(pipol + rc.ang + rc.fi0 / 2),rc.y+r*Math.sin(pipol + rc.ang + rc.fi0 / 2),1,0,dwapi,false);
	$ctx.closePath();
	$ctx.fillStyle='#a00';
	$ctx.fill();
	r = rc.r(-rc.ang);
	$ctx.beginPath();
	$ctx.arc(rc.x + r * Math.cos(-rc.ang),rc.y + r * Math.sin(-rc.ang),1,0,dwapi,false);
	$ctx.closePath();
	$ctx.fillStyle='#00a';
	$ctx.fill();
}

function drawCircle(bl, color){
	$ctx.beginPath();
	$ctx.arc(bl.x,bl.y,bl.r(),0,dwapi,false);
	$ctx.closePath();
	$ctx.fillStyle = color;
	$ctx.fill();
}

function drawPlayer(Pl, color){
	if(color != undefined){
		$ctx.beginPath();
		$ctx.arc(Pl.x, Pl.y, PlayerSize, 0, dwapi, false);
		$ctx.closePath();
		$ctx.fillStyle=color;
		$ctx.fill();
		$ctx.moveTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		$ctx.beginPath();
		$ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang - pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang - pi34));
		$ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang + pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang + pi34));
		$ctx.lineTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		$ctx.closePath();
		$ctx.fillStyle="#000";
		$ctx.fill();
		$ctx.stroke();
	}
	else{
		$ctx.beginPath();
		$ctx.arc(Pl.x,Pl.y,PlayerSize,0,dwapi,false);
		$ctx.closePath();
		$ctx.fillStyle="#000";
		$ctx.fill();
		$ctx.beginPath();
		$ctx.arc(Pl.x,Pl.y,PlayerSize * 0.66,0,dwapi,false);
		$ctx.closePath();
		$ctx.fillStyle="#fff";
		$ctx.fill();
		$ctx.beginPath();
		$ctx.arc(Pl.x,Pl.y,PlayerSize * 0.33,0,dwapi,false);
		$ctx.closePath();
		$ctx.fillStyle="#000";
		$ctx.fill();
		$ctx.moveTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		$ctx.beginPath();
		$ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang - pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang - pi34));
		$ctx.lineTo(Pl.x + PlayerSize * 0.33 * Math.cos(Pl.ang + pi34), Pl.y + PlayerSize * 0.33 * Math.sin(Pl.ang + pi34));
		$ctx.lineTo(Pl.x + PlayerSize * 0.66 * Math.cos(Pl.ang), Pl.y + PlayerSize * 0.66 * Math.sin(Pl.ang));
		$ctx.closePath();
		$ctx.fillStyle="#f00";
		$ctx.fill();
		$ctx.stroke();
	}
}

function drawEntrance(Ent, color){
	var temp;
	if(color === undefined){
		color = Ent.color;
	}
	drawRect(Ent, color);
	temp = Ent.r0;
	Ent.r0 *= 0.66;
	drawRect(Ent, '#000');
	Ent.r0 = temp;
	$ctx.fillStyle = "#f00";
	$ctx.fillText('IN', Ent.x - 10, Ent.y + 10);
}

function drawExit(Ext, color){
	var temp;
	if(color === undefined){
		color = Ext.color;
	}
	drawRect(Ext, color);
	temp = Ext.r0;
	Ext.r0 *= 0.66;
	drawRect(Ext, '#000');
	Ext.r0 = temp;
	$ctx.fillStyle = "#f00";
	$ctx.fillText('OUT', Ext.x - 10, Ext.y + 10);
}

function drawSwitch(Sw, color){
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
			drawRect(Op, color);
			
			
			Op.x += 2 * temp * Math.cos(-Sw.ang);
			Op.y += 2 * temp * Math.sin(-Sw.ang);
			drawRect(Op, color);
			
			Op = rectangle(Sw);
			Op.a *= 0.1;
			Op.b *= 0.8;
			Op = rectangle(Op);
			
			temp = Sw.a * 0.45;
			ang = pipol - Sw.ang;
			
			Op.x -= temp * Math.cos(ang);
			Op.y -= temp * Math.sin(ang);
			drawRect(Op, "#f00");
				
			Op.x += 2 * temp * Math.cos(ang);
			Op.y += 2 * temp * Math.sin(ang);
			drawRect(Op, "#0f0");
			
			temp = Sw.a * 0.7 * Sw.pos / MaxPos - Sw.a * 0.35;
			Op.x = Sw.x + temp * Math.cos(ang);
			Op.y = Sw.y + temp * Math.sin(ang);
			drawRect(Op, color);
			break;
		default:
			drawRect(Sw, color);
	}
}

function drawDoor(Dr, color){
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
			drawRect(Op, color);
			
			Op = rectangle(Dr);
			temp = Dr.b * 0.45;
			Op.b *= 0.1;
			Op.x -= temp * Math.cos(-Op.ang);
			Op.y -= temp * Math.sin(-Op.ang);
			Op = rectangle(Op);
			drawRect(Op, color);
			
			Op.x += 2 * temp * Math.cos(-Op.ang);
			Op.y += 2 * temp * Math.sin(-Op.ang);
			drawRect(Op, color);
			break;
		default:
			drawRect(Dr, color);
	}
	
}

function drawParticle(P, color){
	drawCircle(P, color);
	$ctx.fillStyle = "#000";
	
	if(P.charge === 0){
		color = 'n';
	}else if(P.charge > 0){
		color = '+';
	}else if(P.charge < 0){
		color = '-';
	}
	$ctx.fillText(color, P.x - 8, P.y + 8);
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

function klawa(e){
	if(!e){
		e = window.event;
	}

	switch(e.keyCode){
		case 46://del
			delObject();
			break;
		case 67://shift + c
			if(e.shiftKey){
				cloneObject(-30, 30);
			}
			break;
		case 78://shift + n
			if(e.shiftKey){
				newObject();
			}
			break;
		case 107://+
			newObject();
			break;
		case 76://shift + l
			if(e.shiftKey){
				Load();
			}
			break;
		case 83://shift + s
			if(e.shiftKey){
				GenerateOutput();
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

function selectObjOnCanvas(e){
	if (!e) e = window.event;
	var relTarg = e.relatedTarget || e.fromElement,
		 Obj = false,
		 index,
		 i;

	Data.mouseDragX = e.pageX;
	Data.mouseDragY = e.pageY;
	
	for(i in Data.Arrays){
		if(!Obj){
			for(index in Data[Data.Arrays[i]]){
				if(isClicked(Data[Data.Arrays[i]][index],e)){
					Obj = Data[Data.Arrays[i]][index];
					break;
				}
			}
		}
		else{
			break;
		}
	}
	
	if(!Obj){
		Data.Clicked = {};
	}else{
		Data.Clicked = Obj;
	}
	
	if(Data.Selected.length === 0){
		Data.IsDragging = false;
	}else{
		if(!Obj){
			Data.IsDragging = false;
		}else{
			Data.IsDragging = false;
			for(i in Data.Selected){
				if(Data.Selected[i] === Obj){
					Data.IsDragging = true;
					break;
				}
			}
		}
	}
}

function dropOnCanvas(e){
	if (!e) e = window.event;
	if(Data.IsDragging){
		Data.IsDragging = false;
		Data.mouseDragX = e.pageX - Data.mouseDragX;
		Data.mouseDragY = e.pageY - Data.mouseDragY;
		if(e.ctrlKey){
			cloneObject(Data.mouseDragX, Data.mouseDragY);
		}
		else{
			for(var i in Data.Selected){
				Data.Selected[i].x += Data.mouseDragX;
				Data.Selected[i].y += Data.mouseDragY;
			}
			ReDraw();
		}
		ShowObj(Data.Selected);
	}
	else{
		var x = e.pageX - Data.mouseDragX,
			 y = e.pageY - Data.mouseDragY,
			 SelectionRect = rectangle({x: (e.pageX + Data.mouseDragX) / 2,
												 y: (e.pageY + Data.mouseDragY) / 2,
												 a: y,
												 b: x,
												 ang: 0,
												 color: SelectionColor
												}),
		i, j, ang;
		Data.LastSelected = Data.Selected;
		Data.Selected = [];
		
		for(i in Data.Arrays){
			for(j in Data[Data.Arrays[i]]){
				x = SelectionRect.x - Data[Data.Arrays[i]][j].x;
				y = SelectionRect.y - Data[Data.Arrays[i]][j].y;
				ang = VectorAtan(x, y);
				ang = Math.sqrt(x * x + y * y) - (SelectionRect.r(ang) + Data[Data.Arrays[i]][j].r(norm(ang + pi)));
				if(ang < 0){
					Data.Selected.push(Data[Data.Arrays[i]][j]);
				}
			}
		}
		
		if(Data.Clicked.x != undefined){
			j = true;
			for(i in Data.Selected){
				if(Data.Selected[i] === Data.Clicked){
					j = false;
					break;
				}
			}
			if(j){
				Data.Selected.push(Data.Clicked);
			}
		}
		
		ShowObj(Data.Selected);
		drawSelection();
	}

	SetSelectedOnListFromData();

}

//-------------------------------------------------------------

function sortfunc(a,b){
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
		return 1.5 * pi;
    }
}

//var b; for(var i = -100; i < 100; i+= 0.1){ for(var j = -100; j < 100; j+= 0.1){ b = VectorAtan(i, j); if((b > dwapi) || (b < 0)) console.log([i, j, b]);}}
//no logs, no need to normalize returned angle