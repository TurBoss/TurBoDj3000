function TurBoDj() {}

TurBoDj.init = function(id){
    //print ("Initalizing Reloop Digital Jockey 2 Controler Edition.");
    TurBoDj.resetLEDs();
    TurBoDj.setLEDs();
    
    // map engine stuff for MIDI out
    engine.connectControl("[Channel1]","play","TurBoDj.isChannelPlaying");
    engine.connectControl("[Channel2]","play","TurBoDj.isChannelPlaying");
    
    engine.connectControl("[Channel1]","pfl","TurBoDj.isChannelHeadphonesOn");
    engine.connectControl("[Channel2]","pfl","TurBoDj.isChannelHeadphonesOn");
}

TurBoDj.shutdown = function(id){
    TurBoDj.resetLEDs();
}

TurBoDj.Leds = {
  
  // left deck
  "[Channel1]" : {
      'scratching':	0,
      'play':		1,
      'headphone':	2,
  },
  
  // right deck
  "[Channel2]": {
      'scratching':	20,
      'play':		21,
      'headphone':	22,
  }
};

TurBoDj.caseLeds     = 99;
TurBoDj.firstDeck     = 0;
TurBoDj.pitchDeck     = [0, 0];

TurBoDj.resetLEDs = function() {
    TurBoDj.sendLED(TurBoDj.caseLeds,127);
}

TurBoDj.setLEDs = function() {
    TurBoDj.sendLED(TurBoDj.firstDeck,127);
}

TurBoDj.currentDeck = function(group) {
   if (group == "[Channel1]"){
      return 1;
   }
   else if (group == "[Channel2]"){
      return 2;
   }
   print("Invalid group : " + group);
   return -1; // error
}

TurBoDj.wheelTouch = function (channel, control, value, status, group) {
    //print("wheelTouch: "+group);
    if (value == 0x7F) {
        var alpha = 1.0/8;
        var beta = alpha/32;
        engine.scratchEnable(TurBoDj.currentDeck(group), 128, 33+1/3, alpha, beta);
    }
    else {    // If button up
        engine.scratchDisable(TurBoDj.currentDeck(group));
    }
}

TurBoDj.wheelTurn = function (channel, control, value, status, group) {
    //print("wheelTurn: " + group);
    var newValue = 0;
    
    if (value == 0){
    	newValue = -1;
    }
    
    else if (value == 1){
   	newValue = 1;
    }
   	
    if (!engine.isScratching(1)){
    	engine.setValue(group, "jog", newValue);
    	return;
    }
    
    engine.scratchTick(TurBoDj.currentDeck(group) ,newValue);
}

TurBoDj.pitchTurn = function (channel, control, value, status, group) {
    //print("pitchTurn: " + group);
    var newValue = 0.001;
    
    if (value == 0){
    	TurBoDj.pitchDeck[channel] -= newValue;
    }
    
    else if (value == 1){
   	TurBoDj.pitchDeck[channel] += newValue;
    }
    engine.setValue(group, "rate", TurBoDj.pitchDeck[channel]);
}

TurBoDj.sendLED = function(lednr, val) {
    midi.sendShortMsg(0x90, lednr, val);
}
TurBoDj.sendLEDon = function(group, ledname) {
    midi.sendShortMsg(0x90, TurBoDj.getLedNr(group,ledname), 1);
}

TurBoDj.sendLEDoff = function(group, ledname) {
    midi.sendShortMsg(0x80, TurBoDj.getLedNr(group,ledname), 1);
}

TurBoDj.getLedNr = function(group,ledname) {
    return TurBoDj.Leds[group][ledname];
}



TurBoDj.isChannelPlaying = function (value, group){
    if(value == 0){
	TurBoDj.sendLEDoff(group,'play');
    }
    else{ //if deck is playing 
	TurBoDj.sendLEDon(group,'play');
    }
}

TurBoDj.isChannelHeadphonesOn = function(value, group) {
    TurBoDj.ledSwitcher(group,'headphone',value);
}




TurBoDj.ledSwitcher = function(group, ledname, value) {
    if(value == 0) {
	TurBoDj.sendLEDoff(group,ledname);
    } else {
	TurBoDj.sendLEDon(group,ledname);
    }
}




