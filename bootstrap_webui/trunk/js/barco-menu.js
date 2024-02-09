//////////////////////////////////////////////////////////////////
// Menu Version 1.0.6 - copyright Barco - webmaster@barco.com   //
// Katrien Vanoosthuyse                                         //
// Geert Van Couteren 											//
// Tomas Vandenabeele 											//
// BarcoMenu - 06 July 2004                                     //
// 2.0.3 - Activate changed - (01.03.2007)                      //
// 2.0.2 - Activate changed - (01.03.2007)                      //
// 1.0.4 Problem subclose (06.07.2004)							//
// 1.0.3 Cookie Memory (05.05.2003)                             //
// 1.0.2 Up-Down Images (IE 5+ & NS 6+ )                        //
//////////////////////////////////////////////////////////////////
barcomenu();

function barcomenu()
{
	
	var menu;
	var obj = document.getElementById('menu');
	
	if (document.addEventListener)
	{ //netscape
		obj.addEventListener('click',action, false);
		obj.addEventListener('mouseover',mover, false);
		obj.addEventListener('mouseout',mout, false);
	}
	if (document.attachEvent)
	{ //IE 5+;
		obj.attachEvent("onmousedown",action);
		obj.attachEvent("onmouseover",mover);
		obj.attachEvent("onmouseout",mout);
	}
	
	var doccookies = document.cookie;
	var doccookie = doccookies.split("; ");
	var cookie;
	var resultcookie;
	var restcookie;
	var span;
	var tstje;
	var tstje2;
	
	for (var i in doccookie)
	{
		cookie = doccookie[i].split("=");
		if ((cookie[0] == "barco") && (cookie[1] != ""))
		{
			resultcookie = cookie[1].split(":");
			for (var b in resultcookie)
			{
				restcookie = resultcookie[b].split("#");
				span = document.getElementById(restcookie[0]);
				if (span != null)
				{
					obj=document.getElementById(restcookie[0]);
					if ((!obj.style.display) && (obj.className == 'hide'))
					{
						obj.style.display = 'none';
					}		
					if ((!obj.style.display) && (obj.className == 'show'))
					{
						obj.style.display = 'block';
					}
					//alert (span.id);
					if (restcookie[1] == 1)
					
					{
						
						if (obj.style.display = 'none')
						{	
							tstje = restcookie[0].substring(0,restcookie[0].length -1);
							activate(restcookie[0],tstje);
						}
					}
					else
					{
						if (obj.style.display = 'block' )
						{
							tstje2 = restcookie[0].substring(0,restcookie[0].length -1);
							activate(restcookie[0],tstje2);
						}
					}
				}
			}
		}
	}
}

function action(evt)
{
	
	evt = (evt) ? evt : (window.event) ? window.event : "";
     var elt;
	 if (evt.srcElement) {
        elt = evt.srcElement;
     } else if (evt.target) {
        elt = evt.target;
     }

	
	if(elt.parentNode.tagName=="LI")
	{
		if(elt.getElementsByTagName('a'))
		{
			var golink=elt.getElementsByTagName('a');
			if (golink.length == 1  )
			{
				document.location=golink.item(0).href;
			}
		}
	}
}

function mover(evt)
{
	evt = (evt) ? evt : (window.event) ? window.event : "";
     var elt;
	 if (evt.srcElement) {
        elt = evt.srcElement;
     } else if (evt.target) {
        elt = evt.target;
     }
	
	if(elt.className == 'menulevel2link')
	{
		elt.className = 'menulevel2guide';
	}else if(elt.className == 'menulevel2open'){
		elt.className = 'menulevel2openguide';
	}else if(elt.className == 'menulevel2close'){
		elt.className = 'menulevel2closeguide';
	}
	
	if(elt.parentNode.className == 'menulevel2link')
	{
		elt.parentNode.className = 'menulevel2guide';
	}else if(elt.parentNode.className == 'menulevel2open'){
		elt.parentNode.className = 'menulevel2openguide';
	}else if(elt.parentNode.className == 'menulevel2close'){
		elt.parentNode.className = 'menulevel2closeguide';
	}	

	if(elt.parentNode.parentNode.className == 'menulevel2link')
	{
		elt.parentNode.parentNode.className = 'menulevel2guide';
		
	}else if(elt.parentNode.parentNode.className == 'menulevel2open'){
		elt.parentNode.parentNode.className = 'menulevel2openguide';
	}else if(elt.parentNode.parentNode.className == 'menulevel2close'){
		elt.parentNode.parentNode.className = 'menulevel2closeguide';
	}
}

function mout(evt)
{
	evt = (evt) ? evt : (window.event) ? window.event : "";
     var elt;
	 if (evt.srcElement) {
        elt = evt.srcElement;
     } else if (evt.target) {
        elt = evt.target;
     }
	if(elt.parentNode.parentNode.className == 'menulevel2guide')
	{
		elt.parentNode.parentNode.className = 'menulevel2link';
		
	}else if(elt.parentNode.parentNode.className == 'menulevel2openguide'){
		elt.parentNode.parentNode.className = 'menulevel2open';
	}else if(elt.parentNode.parentNode.className == 'menulevel2closeguide'){
		elt.parentNode.parentNode.className = 'menulevel2close';
	}
	 
	if(elt.parentNode.className == 'menulevel2guide')
	{
		elt.parentNode.className='menulevel2link';
	}else if(elt.parentNode.className == "menulevel2openguide"){
		elt.parentNode.className = "menulevel2open";
	}else if(elt.parentNode.className == "menulevel2closeguide"){
		elt.parentNode.className = "menulevel2close";
	}
	
	if(elt.className == 'menulevel2guide')
	{
		elt.className='menulevel2link';
	}else if(elt.className == "menulevel2openguide"){
		elt.className = "menulevel2open";
	}else if(elt.className == "menulevel2closeguide"){
		elt.className = "menulevel2close";
	}
}


function subclose(target)
{
	var z = 1;
	if (document.getElementById(target))
	{
		var span = document.getElementById(target);
		var img = span.getElementsByTagName('img');
		if (img.length > 0)
		{
			var image=img.item(0).src;
			var rExp = /up.gif/gi;
			var results = image.replace(rExp, new String("down.gif"))
			img.item(0).src=results;
		}
	}
	var obj;
	while(document.getElementById(target+z)!=null) 
	{
		obj = document.getElementById(target+z);
		obj.style.display= 'none';
		SetCookie(target+z,false);
		z++;
	}
}

function toggle(target)
{
	var k = 1;
	if (document.getElementById(target))
	{
		var span = document.getElementById(target);
		if(span.className == "menulevel1close"){
			span.className = "menulevel1open";
		}else if(span.className == "menulevel1open"){
			span.className = "menulevel1close";
		}else if(span.className == "menulevel2openguide"){
			span.className = "menulevel2closeguide";
		}else if(span.className == "menulevel2closeguide"){
			span.className = "menulevel2openguide";
		}
		
		var img=span.getElementsByTagName('img');
		//alert(img);
		var results;
		var rExp;
		if (img.length > 0)
		{
			var image=img.item(0).src;
			if (image.indexOf("up.gif")>0)
			{
				rExp = /up.gif/gi;
				results = image.replace(rExp, new String("down.gif"))
			}	
			else
			{
				rExp = /down.gif/gi;
				results = image.replace(rExp, new String("up.gif"))
			}
			img.item(0).src=results;
		}
	}
	
	var obj;
	while(document.getElementById(target+k)!=null) 
	{
		obj=document.getElementById(target+k);
		if ((!obj.style.display) && (obj.className == 'hide'))
		{
			obj.style.display = 'none';
		}
		
		if ((!obj.style.display) && (obj.className == 'show'))
		{
			obj.style.display = 'block';
		}	
				
		if (obj.style.display=='none')
		{
			obj.style.display = 'block';
			SetCookie(target+k,true);
		}
		else
		{
			if (obj.style.display=='block')
			{
				obj.style.display = 'none';
				subclose(target+k);
				SetCookie(target+k,false);
			}
		}
		k++;
		
	}
}

function activate(target,image)
{
 	var k = 1;
	var obj;
	var span;
	var img;
	var rExp;
	var results;
	if (document.getElementById(image))
	{
		obj=document.getElementById(target);
		span = document.getElementById(image);
		img=span.getElementsByTagName('img');
		
		if (img.length > 0)
		{
			image=img.item(0).src;
			if ((image.indexOf("up.gif")>0)&&(obj.style.display == 'block'))
			{
				rExp = /up.gif/gi;
				results = image.replace(rExp, new String("down.gif"));
				img.item(0).src=results;
			}	
			if ((image.indexOf("down.gif")>0)&&(obj.style.display == 'none'))
			{
				rExp = /down.gif/gi;
				results = image.replace(rExp, new String("up.gif"));
				img.item(0).src=results;
			}
		
		}
	}

	obj=document.getElementById(target);
	
	if ((!obj.style.display) && (obj.className == 'hide'))
	{
		obj.style.display = 'none';
		if ((span.className == "menulevel1open")) span.className = "menulevel1close";
		if ((span.className == "menulevel2open")) span.className = "menulevel2close";
	}
	
	if ((!obj.style.display) && (obj.className == 'show'))
	{
		obj.style.display = 'block';
		if ((span.className == "menulevel1close")) span.className = "menulevel1open";
		if ((span.className == "menulevel2close")) span.className = "menulevel2open";
	}	
	
	if (obj.style.display=='none')
	{
		obj.style.display = 'block';	
		if ((span.className == "menulevel1close")) span.className = "menulevel1open";
		if ((span.className == "menulevel2close")) span.className = "menulevel2open";
	}
	else
	{
		if (obj.style.display=='block')
		{
		obj.style.display = 'none';
		if ((span.className == "menulevel1open")) span.className = "menulevel1close";
		if ((span.className == "menulevel2open")) span.className = "menulevel2close";
		}
	}
	
}

function openatstart(target)
{
toggle(target);
}

function SetCookie (name, value)
{
	var val=value ? '1' : '0';
	var name2 = "barco";
	var listcookies = document.cookie;
	var listcookies1 = listcookies.split("; ");
	var result = name+"#"+val+":";
	var ok = 0; 
	var rest = "";
	var listcookies3;
	var listcookies2; 
	var listcookies4;
	
	var b, c;
	for (c=0; c<listcookies1.length; c++)

	{
		var listcookies2 = listcookies1[c].split("=");
		if (listcookies2[0] == "barco")
		{
			rest = listcookies2[1];
			var listcookies3 = listcookies2[1].split(":");
			for (b=0;b<listcookies3.length; b++)
			{
				var listcookies4 = listcookies3[b].split("#");
				if (listcookies4[0]+"#" == name+"#")
				{
					var result = listcookies2[1].replace(name+"#"+listcookies4[1], name+"#"+val);
					ok = 1;
				}

			}
		}
	}
	if (ok == 0)
	{
		if (rest != "")
			result = rest + name+"#"+val+":";
	}
	document.cookie=name2+'='+result+';path=/';
}