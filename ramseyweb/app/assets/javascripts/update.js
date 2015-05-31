$(document).ready(function(){
	var i=0;
    setInterval(function(){updater(i++);},5000);
    alert("hi there initieal");	
  });

function updater(i){
	//document.getElementById("a").innerHTML=i;
  	$.getScript('welcome/up.js');
};