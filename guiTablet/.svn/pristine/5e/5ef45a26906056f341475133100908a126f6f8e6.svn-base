/**
 * @author ZHEN
 */
Ext.define('TabletGui.controller.DisplayConfig', {
  extend: 'Ext.app.Controller',
  alias: 'controller.displayconfig',
  init:function(){
  },        
  launch:function(){
    
  },
  config: {
    //one dest with one/multiple windows
    destWindowArr:[]
  },
    
	//apply display config
	applyDisplayCofig:function(displayCfgNode,cavasDivDom){	
		var canvasNode = displayCfgNode.data.canvas,
			  nid = displayCfgNode.data.nid;
		this.applyCanvas(nid,canvasNode,cavasDivDom);		
	},
	
	//apply canvas
	applyCanvas:function(nid,canvasNode,cavasDivDom){
		var cvsWidth = canvasNode['@hres'],
  			cvsHeight = canvasNode['@vres'],
  			divWidth = cavasDivDom.clientWidth,
  			divHeight = cavasDivDom.clientHeight,
  			windowNode = canvasNode.Window,
  			layoutCssCls = '',
  			winId = '',
  			dominatWidth = 0,
  			dominatHeight = 0,
  			convertRatio = 0;
  	console.log('DisplayConfig:applyCanvas:cvsWidth '+cvsWidth+', cvsHeight: '+cvsHeight);
		//find dominate orientation; preset divWidth:divHeight = 16:9
		if(cvsWidth/cvsHeight < divWidth/divHeight)	 {
			// adjust divWidth to meet cvsWidth/cvsHeight ration
			dominatWidth = parseInt(divHeight * cvsWidth/cvsHeight);
			dominatHeight = divHeight;
		}
		else{
			//adjust cvsHeight to meet cvsWidth/cvsHeight ration
			dominatWidth = divWidth ;
			dominatHeight = parseInt(divWidth * cvsHeight/cvsWidth);
		}
		convertRatio = dominatWidth/cvsWidth; //same as dominatHeight/cvsHeight
		cavasDivDom.clientWidth = dominatWidth;
		cavasDivDom.style.clientHeight = dominatHeight;		
		
		var dstTplArr = [],
		    destWinArr = [];
		if(windowNode && (windowNode instanceof Array)){
  		Ext.iterate(windowNode,function(winObj){
        var winId = winObj['@id'],
          	propertyNode = winObj.Properties,
          	hpos = propertyNode['@hpos']+'',
          	hres = propertyNode['@hres']+'',
          	vpos = propertyNode['@vpos']+'',
          	vres = propertyNode['@vres']+'',
          	zIndex = propertyNode['@zIndex']+'',
          	visibility = propertyNode['@visibiity'],
          	scale = propertyNode['@scale'];        
        //bind destNID and winId for ROC   	
        destWinArr.push({"nid":nid,"windowid":winId});         
				hpos = (hpos == 0)||(hpos.indexOf('%') > 0) ? hpos : parseInt(parseInt(hpos)*convertRatio)+'px',
      	vpos = (vpos == 0)||(vpos.indexOf('%') > 0) ? vpos : parseInt(parseInt(vpos)*convertRatio)+'px',      						
      	hres = (hres == 0)||(hres.indexOf('%') > 0) ? hres : parseInt(parseInt(hres)*convertRatio)+'px',
      	vres = (vres == 0)||(vres.indexOf('%') > 0) ? vres : parseInt(parseInt(vres)*convertRatio)+'px',      				        
        inlineStyle = 'position:absolute;top:'+vpos+';left:'+hpos+';height:'+vres+';width:'+hres+';zIndex:'+zIndex+';';	        	
        dstTplArr.push(' <div id="'+ winId +'" class="display-window\" style=\"'+inlineStyle+'\" >');
				dstTplArr.push(' <img src=" " alt="'+winId+'" style="width:100%; height:100%; !important" id=\"img_'+nid+'\">');
        dstTplArr.push(' </div>');
	    }.bind(this)); 		
		}
		else {
			var winId = windowNode['@id'] !="id" ? windowNode['@id']: 'No_'+nid;			
			//bind destNID and winId for RX
			destWinArr.push({"nid":nid,"windowid":winId});			
			//if one window like Rx/Xp
			var styleCSS = 'position:absolute;top:0;left:0;height:100%;width:100%;zIndex:1';
			dstTplArr.push(' <div class=\"display-window\" id=\"'+winId+'\" style=\"'+styleCSS+'\">');
			dstTplArr.push(' <img src=" " style="width:100%; height:100%; !important" id=\"img_'+nid+'\">');
			dstTplArr.push(' </div>');
		}
		cavasDivDom.innerHTML = dstTplArr.join('');
		//keep destNID-winId 
		this.setDestWindowArr(destWinArr);
	},

	//apply windows
	applyWindows:function(windowNode,aspecRatio){
		
	},
			
	//apply display status
	applyDisplayStatus:function(displayCfgStatus){
	
	}
  //dstListItemDom.innerHTML = dstHtmlTpl;
});

/*
 var parentDomNode = cavasDivDom.parentNode;
		if(windowNode && (windowNode instanceof Array)){
			parentDomNode.removeChild(cavasDivDom);//TBR
		  		Ext.iterate(windowNode,function(winObj){
			        var winId = winObj['@id'],
			        	propertyNode = winObj.Properties,
			        	hpos = propertyNode['@hpos']+'',
			        	hres = propertyNode['@hres']+'',
			        	vpos = propertyNode['@vpos']+'',
			        	vres = propertyNode['@vres']+'',
			        	zIndex = propertyNode['@zIndex']+'',
			        	visibility = propertyNode['@visibiity'],
			        	scale = propertyNode['@scale'];

					hpos = (hpos == 0)||(hpos.indexOf('%') > 0) ? hpos : parseInt(parseInt(hpos)*convertRatio)+'px',
		        	vpos = (vpos == 0)||(vpos.indexOf('%') > 0) ? vpos : parseInt(parseInt(vpos)*convertRatio)+'px',
		        						
		        	hres = (hres == 0)||(hres.indexOf('%') > 0) ? hres : parseInt(parseInt(hres)*convertRatio)+'px',
		        	vres = (vres == 0)||(vres.indexOf('%') > 0) ? vres : parseInt(parseInt(vres)*convertRatio)+'px',
		        				        
			        inlineStyle = 'position:absolute;top:'+vpos+';left:'+hpos+';height:'+vres+';width:'+hres+';zIndex:'+zIndex+';';
					var wrapper= document.createElement('div');	
					wrapper.setAttribute('class', 'display-window');
					wrapper.setAttribute('style', inlineStyle);	
					wrapper.innerText = winId;
		        parentDomNode.appendChild(wrapper);
			    }.bind(this)); 		
		}		
 */
