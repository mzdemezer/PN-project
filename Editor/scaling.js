var scale = 1024 / 750,
    i, j;
	 
for(i in Data.Arrays){
	for(j in Data[Data.Arrays[i]]){
		Data[Data.Arrays[i]][j].x *= scale;
		Data[Data.Arrays[i]][j].y *= scale;
		if(Data[Data.Arrays[i]][j].r0 != undefined){
			Data[Data.Arrays[i]][j].r0 *= scale;
		}
		if(Data[Data.Arrays[i]][j].a != undefined){
			Data[Data.Arrays[i]][j].a *= scale;
			Data[Data.Arrays[i]][j].b *= scale;
		}
		switch(Data[Data.Arrays[i]][j].type){
			case "Circle": 
				Data[Data.Arrays[i]][j] = circle(Data[Data.Arrays[i]][j]);
				break;
			case "Square":
				Data[Data.Arrays[i]][j] = square(Data[Data.Arrays[i]][j]);
				break;
			case "Rectangle":
				Data[Data.Arrays[i]][j] = rectangle(Data[Data.Arrays[i]][j]);
				break;
			case "Player":
				Data[Data.Arrays[i]][j] = player(Data[Data.Arrays[i]][j]);
				break;
			case "Entrance":
				Data[Data.Arrays[i]][j] = entrance(Data[Data.Arrays[i]][j]);
				break;
			case "Exit":
				Data[Data.Arrays[i]][j] = exit(Data[Data.Arrays[i]][j]);
				break;
			case "Switch":
				Data[Data.Arrays[i]][j] = aswitch(Data[Data.Arrays[i]][j]);
				break;
			case "Door":
				Data[Data.Arrays[i]][j] = door(Data[Data.Arrays[i]][j]);
				break;
			case "Particle":
				Data[Data.Arrays[i]][j] = particle(Data[Data.Arrays[i]][j]);
				break;
		}
	}	
}
ReDraw();