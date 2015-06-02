$(document).ready(function(){
	var i=0;
	updater(i);
    //setInterval(function(){updater(i++);},4000);
    //alert("welcome partyof4. be patient.");	
  });

function updater(i){
  	$.getScript('welcome/up.js');
};