var marker, map, area, setarea, check = 0, checkI = 0;
var Markers = new Array();
var mqtt;
var test;
var reconnectTimeout = 2000;
var host="120.126.136.17";
var port=11883;
var Friends = new Array();
var user = "blank";


function initialize(posi,id) {
    if(check == 0){
        var myLatlng = new google.maps.LatLng(24.944, 121.3695);
        var mapOptions = {
          zoom: 16.5,
          center: myLatlng,
          zoomControl: true,
          scaleControl: false,
          rotateControl: false,
          mapTypeControl: false,
          streetViewControl: false,
          fullscreenControl: false,
          mapTypeId: google.maps.MapTypeId.ROADMAP
        }
        map = new google.maps.Map(document.getElementById('map_canvas'), mapOptions);
        check = 1;
    } 
    else{
        var u = './static/images/user/'+ Friends[id] +'.jpg';
        if(checkI == 0){
          marker = new SlidingMarker({
            position: posi,
            map: map,
            icon: {
              url : u,
              scaledSize: new google.maps.Size(40,45)     
            },
            title: Friends[id]
          });
        }
        /*else if(checkI == 1){
          Markers[id].setMap(null);
          u = 'http://maps.google.com/mapfiles/ms/micons/caution.png';
          marker = new SlidingMarker({
            position: posi,
            map: map,
            icon: {
              url : u,
              scaledSize: new google.maps.Size(40,45)              
            },
            title: name
          });

          var contentString = '<p style = "color:#FF0000">使用者超出了安全範圍</p>'
          var infowindow = new google.maps.InfoWindow({
            content: contentString,
            position: posi,
            maxwidth: 100,
          });
          infowindow.open(map,marker);
        }*/
        Markers[id] = marker;

        var $log = $("#log");
                
        $log.html(
            "<b>left click</b> to call setPosition<br/>" + 
            "<b>right click</b> to call setPositionNotAnimated<br/>");
                
        google.maps.event.addListener(marker, 'position_changed', function () {
            $log.html($log.html() + "marker.position_changed<br/>");
        });
    }

}

area = [
  {lat: 24.9441713, lng:121.368987}, 
  {lat: 24.9435342, lng:121.3694215},
  {lat: 24.9440984, lng:121.3703978},
  {lat: 24.9447307, lng:121.3699418}, 
  {lat: 24.9441713, lng:121.368987}, 
  ];

function set_area(){
  if(!setarea){
    setarea = new google.maps.Polygon({
      paths: area,  
      strokeColor: '#FF0000',  
      strokeOpacity: 0.8,  
      strokeWeight: 3,  
      fillColor: '#FF0000',  
      fillOpacity: 0.05  
    });
    setarea.setMap(map);
  }
  else{
    setarea.setMap(map);
  }
}

function remove_area(){
  setarea.setMap(null);
}

///////////////////////////////////////////////////////////////////////////////////////

$(function () {
    initialize(null);
    console.log("func");
});

/////////////////////////////////////////////////////////////////////////////////////////

function getFriend(number ,friends ,username){
  for (var i = 0 ; i < number.length ; i++){
    Friends[number[i]] = friends[i];
  }
  user = username;
  console.log(Friends);
}

function onFailure(message) {
	console.log("Connection Attempt to Host "+host+"Failed");
	setTimeout(MQTTconnect, reconnectTimeout);
}

function onMessageArrived(msg){
  out_msg=msg.payloadString;
  var temp = out_msg.split(":");
  console.log(out_msg);
  var la = parseFloat(temp[0]);
  var ln = parseFloat(temp[1]);
  var id = temp[2];
  test = {lat: ln, lng: la};
  if (id in Friends){
      if(id in Markers){
        Markers[id].setPosition(test);
      }
      else{
        initialize(test,id);
      }
  }

  set_area();
  /*var change = new google.maps.LatLng(ln,la); //fromat change

  var temppp = google.maps.geometry.poly.containsLocation(change, setarea);
  if(temppp){
    checkI = 0;
    initialize(test,id);
  }
  else{
    if(checkI == 0){
      checkI = 1;
      initialize(test,id)
      alert("使用者超出了安全範圍，請儘速聯絡該使用者提醒他");
    }
  }*/
}

function onConnect() {
    console.log("Connected ");
    mqtt.subscribe("test");
}

function MQTTconnect(user) {
  console.log("connecting to "+ host +" "+ port);
  mqtt = new Paho.MQTT.Client(host,port,user);
  var options = {
    timeout: 3,
    onSuccess: onConnect,
    onFailure: onFailure,
  };
  mqtt.onMessageArrived = onMessageArrived;
      
  mqtt.connect(options); //connect
}