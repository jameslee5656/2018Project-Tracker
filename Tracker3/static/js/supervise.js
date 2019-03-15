var marker, map, area, setarea, check = 0, checkI = 0;
var Markers = new Array();
var mqtt;
var toogle_bracelet
var position;
var reconnectTimeout = 2000;
var host="120.126.136.17";
var port=9001;
var Friends = new Array();
var User = "blank";
var UC = 0; //user counter
var tempdataset_time = ["no data" ,"no data" ,"no data" ,"no data" ,"no data" ,"no data" ,"no data" ,];
var tempdataset_hour = [0,0,0,0,0,0,0];
var tempdataset_minute = [0,0,0,0,0,0,0];
var tempdataset_heartRate = [0,0,0,0,0,0,0];
var tempdataset_bloodOxygen = [0,0,0,0,0,0,0];


function initialize(posi,id) {
    if(check == 0){
        var myLatlng = new google.maps.LatLng(24.944, 121.3695);
        var mapOptions = {
          zoom: 15.5,
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
        var u = './static/images/user/'+ User +'.jpg';
        if(checkI == 0){
          marker = new SlidingMarker({
            position: posi,
            map: map,
            icon: {
              url : u,
              scaledSize: new google.maps.Size(40,45)     
            },
            title: id
          });
        }

        Markers[id] = marker;
        // Markers[UC] = marker;
        // UC++;

        var $log = $("#log");
                
        $log.html(
            "<b>left click</b> to call setPosition<br/>" + 
            "<b>right click</b> to call setPositionNotAnimated<br/>");
                
        google.maps.event.addListener(marker, 'position_changed', function () {
            $log.html($log.html() + "marker.position_changed<br/>");
        });
    }

}

///////////////////////////////////////////////////////////////////////////////////////

$(function () {
    initialize(null);
});

/////////////////////////////////////////////////////////////////////////////////////////

// function getFriend(number ,friends ,username){
//   for (var i = 0 ; i < number.length ; i++){
//     Friends[number[i]] = friends[i];
//   }
//   user = username;
//   console.log(Friends);
// }

function onFailure(message) {
	console.log("Connection Attempt to Host "+host+"Failed");
	setTimeout(MQTTconnect, reconnectTimeout);
}

function onMessageArrived(msg){
  out_msg=msg.payloadString;
  var temp = out_msg.split(":");
  console.log(out_msg);
  var ln = parseFloat(temp[1]);
  var la = parseFloat(temp[2]);
  var steps = parseFloat(temp[5]);
  var heartRate = parseFloat(temp[3]);
  var bloodOxygen = parseFloat(temp[4]);
  var hour = temp[9];
  var minute = temp[10];
  var second = temp[11];
  var time = hour + ":" + minute + ":" + second;
  console.log(time);
  var id = temp[0];
  position = {lat: la, lng: ln};

  if(id in Markers){
    Markers[id].setPosition(position);
  }
  else{
    initialize(position,id);
  }

  for(var i = 0; i < 5; i++){
    tempdataset_time[i] = tempdataset_time[i+1];
    tempdataset_heartRate[i] = tempdataset_heartRate[i+1];
    tempdataset_bloodOxygen[i] = tempdataset_bloodOxygen[i+1];
  }
  tempdataset_time[5] = time;
  tempdataset_heartRate[5] = heartRate;
  tempdataset_bloodOxygen[5] = bloodOxygen;
    


  chart = new Chart(ctx, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: [0,tempdataset_time[0], tempdataset_time[1], tempdataset_time[2], tempdataset_time[3], tempdataset_time[4], tempdataset_time[5]],
        datasets: [{
            label: "心率",
            backgroundColor: 'rgba(76, 138, 150, 0)',
            borderColor: 'rgba(76, 138, 150, 1)',
            data: [0,tempdataset_heartRate[0], tempdataset_heartRate[1], tempdataset_heartRate[2], tempdataset_heartRate[3], tempdataset_heartRate[4], tempdataset_heartRate[5]],
        }]
    },
    // Configuration options go here
    options: {
      tooltips: {
            mode: 'x',
            intersect: 'false'
        }
    }
  });
  chart.update();

  chart2 = new Chart(ctx2, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: [0,tempdataset_time[0], tempdataset_time[1], tempdataset_time[2], tempdataset_time[3], tempdataset_time[4], tempdataset_time[5]],
        datasets: [{
        label: "血氧",
            backgroundColor: 'rgba(76, 138, 150, 0)',
            borderColor: 'rgba(150, 88, 76, 1)',
            data: [0,tempdataset_bloodOxygen[0] ,tempdataset_bloodOxygen[1], tempdataset_bloodOxygen[2], tempdataset_bloodOxygen[3], tempdataset_bloodOxygen[4], tempdataset_bloodOxygen[5]],
        }]
    },
    // Configuration options go here
    options: {
      tooltips: {
            mode: 'x',
            intersect: 'false'
        }
    }
  });
  chart2.update();
}

function onConnect() {
    console.log("Connected ");
    mqtt.subscribe("test");
    // mqtt.subscribe("HR_HIGH");
}

function MQTTconnect(user) {
  User = user;
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

function toogleBracelet(){
  var toogleBraceletMessage = "Shake yourself, Bracelet!";
  toogle_bracelet.send('');
}

function toggleSidebar(){
  document.getElementById("sidebar").classList.toggle('not-visible');
  document.getElementById("caret-left").classList.toggle('caret-right');
}

var ctx = document.getElementById('myChart').getContext('2d');
var ctx2 = document.getElementById('myChart2').getContext('2d');
var chart = new Chart(ctx, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: ["no data", "no data", "no data","no data","no data","no data","no data"],
        datasets: [{
            label: "心率",
            backgroundColor: 'rgba(76, 138, 150, 0)',
            borderColor: 'rgba(76, 138, 150, 1)',
            data: [0, 0, 0, 0, 0, 0, 0],
        }]
    },

    // Configuration options go here
    options: {
      tooltips: {
            mode: 'x',
            intersect: 'false'
        }
    }
});
var chart2 = new Chart(ctx2, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: ["no data", "no data", "no data","no data","no data","no data","no data"],
        datasets: [{
        label: "血氧",
            backgroundColor: 'rgba(76, 138, 150, 0)',
            borderColor: 'rgba(150, 88, 76, 1)',
            data: [0, 0, 0, 0, 0, 0, 0],
        }]
    },

    // Configuration options go here
    options: {
      tooltips: {
            mode: 'x',
            intersect: 'false'
        }
    }
});



//////////////////////////////////////////////////////////////////////////////////