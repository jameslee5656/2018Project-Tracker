var marker, map, area, setarea, check = 0, checkI = 0, open_user_info = 0, userCounter = 0, clearDefaultCheck = 0;
var Markers = new Array(), MarkersArray = new Array();
var mqtt;
var toogle_bracelet
var position;
var reconnectTimeout = 2000;
var host="120.126.136.17";
var port=9001;
var Friends = new Array();
var User = "blank";
var uurl, user_marker_id, user_chart_id;
var tempdataset_time = ["no data" ,"no data" ,"no data" ,"no data" ,"no data" ,"no data" ,"no data" ,];
var tempdataset_hour = [0,0,0,0,0,0,0];
var tempdataset_minute = [0,0,0,0,0,0,0];
var tempdataset_heartRate = [0,0,0,0,0,0,0];
var tempdataset_bloodOxygen = [0,0,0,0,0,0,0];
var normal_color = "rgba(133, 234, 86, 1)";
var notice_color = "rgba(255, 229, 67, 1)";
var alart_color = "rgba(255, 81, 81, 1)";
var direction, directionLa = new Array(), directionLn = new Array();
var directionsService = new google.maps.DirectionsService();
var directionsDisplay = new google.maps.DirectionsRenderer();
var current_user_lat = 0, current_user_lng = 0;
var directionSwitch = 0, directionshowSwitch = 0;
var recommend_name = new Array();
var recommendName, recommendResult = 0;
var currentDireciion, currentMode = 2;
var POI,POIs1 = [], POIs2 = [], POIs3 = [];
var modePOIs2Switch = 0, modePOIs3Switch = 0;
var service;

function initialize(posi,id) {
    if(check == 0){
        var myLatlng = new google.maps.LatLng(24.944, 121.3695);
        var mapOptions = {
          zoom: 15.5,
          minZoom: 8,
          maxZoom: 19,
          center: myLatlng,
          zoomControl: false,
          scaleControl: false,
          rotateControl: false,
          mapTypeControl: false,
          streetViewControl: false,
          fullscreenControl: false,
          mapTypeId: google.maps.MapTypeId.ROADMAP
        }
        map = new google.maps.Map(document.getElementById('map_canvas'), mapOptions);
        check = 1;

        //direction initialize
        directionsDisplay.setMap(map);
    }
    else{
        uurl = "./static/images/user/"+ id +".jpg";
        var heartRateText = '(心律 / 血氧)';
        var u = {
          url: uurl,
          scaledSize: new google.maps.Size(30, 35),
          labelOrigin: new google.maps.Point(17.5,-8)
        };
        if(checkI == 0){
          // marker = new SlidingMarker({
          //   position: posi,
          //   map: map,
          //   icon: u,
          //   title: id,
          //   label: {
          //     text: heartRateText,
          //     color: "rgba(98, 196, 49, 1)",
          //     fontSize: "13px",
          //     fontWeight: "bold"
          //   }
          // });
          user_marker_id = id;

          marker = new RichMarker({
            position: posi,
            map: map,
            draggable: false,
            user_marker_id: user_marker_id,
            uurl: uurl,
            content: '<div class="user_marker user_marker_info_hidden" id="user_marker_'+user_marker_id+'"><div class="marker" style="width: 50px; height:50px; border-radius: 50%;display:inline-block; background-image: url('+uurl+'); background-size: cover; background-position: center;border: 4px solid '+normal_color+';"><div class="user_marker_pointer" style="border-color: '+normal_color+' transparent transparent transparent;"></div></div><div class="user_marker_info">心律:<br>血氧:<br>裝置:</div></div>'
          });

          marker.addListener('click',function(){
              document.getElementById("current_user_pic").style.backgroundImage = "url("+this.uurl+")";
              document.getElementById("current_user_name").innerHTML = this.user_marker_id;
              user_chart_id = this.user_marker_id;
              for(var i = 0; i < 6; i++){
                tempdataset_time[i] = 'no data';
                tempdataset_heartRate[i] = 0;
                tempdataset_bloodOxygen[i] = 0;
              }
              if(user_chart_id == "pitest" || user_chart_id == "pitest2"){
                document.getElementById("bracelet_btn").classList.remove("bracelet_btn_hidden");
                document.getElementById("bracelet_info_p").innerHTML = "裝置: 小米手環 + 樹莓派";
              }
              else{
                document.getElementById("bracelet_btn").classList.add("bracelet_btn_hidden");
                document.getElementById("bracelet_info_p").innerHTML = "裝置: Golife手環 + 手機";
              }
          });
        }

        Markers[id] = marker;
        MarkersArray[userCounter] = marker;
        userCounter++;

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
    toggleRecommend();
    changeRanking();
    recommend();
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
  var heartRate = temp[3]+"";
  var bloodOxygen = temp[4]+"";
  var hour = temp[9];
  var minute = temp[10];
  var second = temp[11];
  var id = temp[0];
  var device = "手機";

  if(id == User){
    current_user_lat = la;
    current_user_lng = ln;
    if(directionSwitch == 1){
      directionshow(currentDireciion,0);
    }
  }

  if(!heartRate){
    heartRate = "心率";
  }
  if(!bloodOxygen){
    bloodOxygen = "血氧";
  }
  if(!hour || !minute || !second){
    time = "no data";
  }
  else{
    time = hour + ":" + minute + ":" + second;
  }
  if(id == "pitest" || id == "pitest2"){
    device = "Pi";
  }
  uurl = "./static/images/user/"+ id +".jpg";
  position = new google.maps.LatLng(la,ln);


  if(id in Markers){
    if(!ln || !la){
      
    }
    else{
      user_marker_id = id;
      Markers[id].setPosition(position);
      var content 
      if(open_user_info == 0){
      content = '<div class="user_marker user_marker_info_hidden" id="user_marker_'+user_marker_id+'"><div class="marker" style="width: 50px; height:50px; border-radius: 50%;display:inline-block; background-image: url('+uurl+'); background-size: cover; background-position: center;border: 4px solid '+normal_color+';"><div class="user_marker_pointer" style="border-color: '+normal_color+' transparent transparent transparent;"></div></div><div class="user_marker_info">心律: '+heartRate+'<br>血氧: '+bloodOxygen+'<br>裝置: '+device+'</div></div>';
      }
      else{
      content = '<div class="user_marker" id="user_marker_'+user_marker_id+'"><div class="marker" style="width: 50px; height:50px; border-radius: 50%;display:inline-block; background-image: url('+uurl+'); background-size: cover; background-position: center;border: 4px solid '+normal_color+';"><div class="user_marker_pointer" style="border-color: '+normal_color+' transparent transparent transparent;"></div></div><div class="user_marker_info">心律: '+heartRate+'<br>血氧: '+bloodOxygen+'<br>裝置: '+device+'</div></div>';
      }
      Markers[id].setContent(content);
    }
  }
  else{
    if(!ln || !la){

    }
    else{
      initialize(position,id);
    }
  }

  if(id == user_chart_id){
    for(var i = 0; i < 5; i++){
      tempdataset_time[i] = tempdataset_time[i+1];
      tempdataset_heartRate[i] = tempdataset_heartRate[i+1];
      tempdataset_bloodOxygen[i] = tempdataset_bloodOxygen[i+1];
    }
    tempdataset_time[5] = time;
    tempdataset_heartRate[5] = heartRate;
    tempdataset_bloodOxygen[5] = bloodOxygen;
  }
    
  if(id == user_chart_id){
    chart.destroy();
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
          },
          {
          label: "血氧",
              backgroundColor: 'rgba(76, 138, 150, 0)',
              borderColor: 'rgba(150, 88, 76, 1)',
              data: [0,tempdataset_bloodOxygen[0] ,tempdataset_bloodOxygen[1], tempdataset_bloodOxygen[2], tempdataset_bloodOxygen[3], tempdataset_bloodOxygen[4], tempdataset_bloodOxygen[5]],
          }]
      },
      // Configuration options go here
      options: {
        tooltips: {
            mode: 'index',
            axis: 'y',
            intersect: false
          }
      }
    });
    chart.update();
  }
}

function onConnect() {
    console.log("Connected ");
    mqtt.subscribe("test");
    // mqtt.subscribe("HR_HIGH");
}

function MQTTconnect(user) {
  User = user;
  user_chart_id = User;
  // console.log("connecting to "+ host +" "+ port);
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
  // var toogleBraceletMessage = "Shake yourself, Bracelet!";
  // toogle_bracelet.send('');
  $.getJSON($SCRIPT_ROOT + '/bracelet', {}, function(data){
    console.log('toogleBracelet');
  });
}

function toggleSidebar(){
  document.getElementById("sidebar").classList.toggle('not-visible');
  document.getElementById("caret-left").classList.toggle('caret-right');
}
function toggleRecommend(){
  document.getElementById("recommend_bar").classList.toggle('not-visible2');
}

var ctx = document.getElementById('myChart').getContext('2d');
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
        },{
        label: "血氧",
            backgroundColor: 'rgba(76, 138, 150, 0)',
            borderColor: 'rgba(150, 88, 76, 1)',
            data: [0, 0, 0, 0, 0, 0, 0],
        }
        ]
    },

    // Configuration options go here
    options: {
      tooltips: {
          mode: 'index',
          axis: 'y',
          intersect: false
        }
    }
});

function user_info_open_or_not(){
  if(open_user_info == 0){
    for(var i = 0; i < MarkersArray.length; i++){
      document.getElementById('user_marker_' + MarkersArray[i].user_marker_id).classList.remove("user_marker_info_hidden");
    }
    open_user_info = 1;
  }
  else{
    for(var i = 0; i < MarkersArray.length; i++){
      document.getElementById('user_marker_' + MarkersArray[i].user_marker_id).classList.add("user_marker_info_hidden");
    }
    open_user_info = 0;
  }
}

function addclass(){
  document.getElementById('uploadPic-con').classList.add('uploadPic-con-show');
}

function remove2(){
  document.getElementById('uploadPic-con').classList.remove('uploadPic-con-show');
}
//////////////////////////////////////////////////////////////////////////////////

var myVar=setInterval(function(){changeRanking()},30000);


// clear the ranking default div
function clearDefault(){
  for(var i = 1; i < 4; i++){
    $("#tempRank" + i).remove();
  }
}
// get ranking and change div
function changeRanking(){
  //getJSON
  var ranking_number = 0;
  var name = [];
  var steps = [];
  var picurl = [];
  $.getJSON($SCRIPT_ROOT + '/GetRanking_api', {}, function(data){
    ranking_number = data.length;
    for(var i = 0; i < ranking_number; i++){
      name[i] = data[i][0];
      steps[i] = data[i][1];
      picurl[i] = data[i][2];
      picurl[i] = 'url(\''+ data[i][2] + '\');';
    }
    if (clearDefaultCheck == 0){
      clearDefault();
      clearDefaultCheck = 1;
      for(var i = 1; i < ranking_number+1; i++){
        $("#ranking-con").append('<div class="friend_ranking-con" id="tempRank'+i+'"><div class="rank-con"><div class="ranking">'+i+'</div></div><div class="ranking-pic" style="background-image: '+picurl[i-1]+'"></div><div class="ranking-name">'+name[i-1]+'</div><div class="ranking-distance">已經走了 '+steps[i-1]+' 步</div><div class="decorate1"></div><div class="decorate2"></div></div>');
        if(i == 1){
            document.getElementById('tempRank1').style.backgroundColor = 'rgba(255, 157, 168, 0.8)';
        }
        else if(i == 2){
            document.getElementById('tempRank2').style.backgroundColor = 'rgba(157, 215, 255, 0.8)';
        }
        else if(i == 3){
            document.getElementById('tempRank3').style.backgroundColor = 'rgba(198, 225, 187, 0.8)';
        }
      }
    }
    else{
      for(var i = 1; i < ranking_number+1; i++){
        $("#tempRank" + i).remove();
      }
      for(var i = 1; i < ranking_number+1; i++){
        $("#ranking-con").append('<div class="friend_ranking-con" id="tempRank'+i+'"><div class="rank-con"><div class="ranking">'+i+'</div></div><div class="ranking-pic" style="background-image: '+picurl[i-1]+'"></div><div class="ranking-name">'+name[i-1]+'</div><div class="ranking-distance">已經走了 '+steps[i-1]+' 步</div><div class="decorate1"></div><div class="decorate2"></div></div>');
        if(i == 1){
            document.getElementById('tempRank1').style.backgroundColor = 'rgba(255, 157, 168, 0.8)';
        }
        else if(i == 2){
            document.getElementById('tempRank2').style.backgroundColor = 'rgba(157, 215, 255, 0.8)';
        }
        else if(i == 3){
            document.getElementById('tempRank3').style.backgroundColor = 'rgba(198, 225, 187, 0.8)';
        }
      }
    }
  });
}

// Google API
// get nearby interest_point from our heat pot
function getPOI(rank,lng,lat){
  //Google Request
  var requestLocation = new google.maps.LatLng(lat,lng);
  var request = {
    location: requestLocation,
    radius: '100',
    type: ['point_of_interest']
  };
  service = new google.maps.places.PlacesService(map);
  service.nearbySearch(request, function(results, status) {
    if (status == google.maps.places.PlacesServiceStatus.OK) {
      POI = results; // all the points of interest
      recommendName = POI[recommendResult].name;
      if(recommendName in recommend_name || !(POI[recommendResult].photos)){
        while(recommendName in recommend_name || !(POI[recommendResult].photos)){
          recommendResult++;
          recommendName = POI[recommendResult].name;
        }
        recommend_name[recommendName] = 1;
      }
      else{
        recommend_name[recommendName] = 1;
      }
      var address = POI[recommendResult].vicinity;
      document.getElementById("R-name"+rank).innerHTML = recommendName;
      document.getElementById("R-address"+rank).innerHTML = address;
      var photo_reference = POI[recommendResult].photos[0].getUrl();
      var picUrl = 'https://maps.googleapis.com/maps/api/place/photo?maxwidth=100&photoreference='+photo_reference+'&key=AIzaSyD_xrySG3MlQuGCwglYYeXztFQehgNGDbw';
      var picUrl = photo_reference;
      document.getElementById("R-pic"+rank).style.backgroundImage = "url('"+picUrl+"')";
      directionLa[rank] = POI[recommendResult].geometry.location.lat();
      directionLn[rank] = POI[recommendResult].geometry.location.lng();
      createRoute(rank,directionLa[rank],directionLn[rank]);
      var current_destination_lat = directionLa[rank];
      var current_destination_lng = directionLn[rank];
      recommendResult = 0;
      var PointStart ={
        lat: current_user_lat,
        lng: current_user_lng
      }
      var PointDestination ={
        lat: current_destination_lat,
        lng: current_destination_lng
      }
      var distance = google.maps.geometry.spherical.computeDistanceBetween(
         new google.maps.LatLng(PointStart),
         new google.maps.LatLng(PointDestination)
      )
      distance = Math.round(distance);
      document.getElementById("R-dis"+rank).innerHTML = '距離' + distance + '公尺';

      POI_point = {
        name: recommendName,
        address: address,
        photo: picUrl,
        lat: directionLa[rank],
        lng: directionLn[rank],
        distance: distance
      }
      POIs2.push({
        name: recommendName,
        address: address,
        photo: picUrl,
        lat: directionLa[rank],
        lng: directionLn[rank],
        distance: distance,
        rank: rank
      });
      POIs3.push({
        name: recommendName,
        address: address,
        photo: picUrl,
        lat: directionLa[rank],
        lng: directionLn[rank],
        distance: distance
      });
    }
  });
}

function preChangePOI_mode(mode){
  modePrechange();
  changePOI_mode(mode);
}

function changePOI_mode(mode){
  if(mode != currentMode){
    if(mode == 2){
      document.getElementById('mode2').classList.add('mode_btn_active');
      document.getElementById('mode3').classList.remove('mode_btn_active');

      var counter = 1;
      for(var i = 0; i < 10; i++){
        document.getElementById("R-name"+(counter)).innerHTML = POIs2[i].name;
        document.getElementById("R-address"+(counter)).innerHTML = POIs2[i].address;
        document.getElementById("R-pic"+(counter)).style.backgroundImage = "url('"+POIs2[i].photo+"')";
        document.getElementById("R-dis"+(counter)).innerHTML = '距離' + POIs2[i].distance + '公尺';
        createRoute(counter,POIs2[i].lat,POIs2[i].lng);
        directionLa[i+1] = POIs2[i].lat;
        directionLn[i+1] = POIs2[i].lng;
        var PointStart ={
          lat: current_user_lat,
          lng: current_user_lng
        }
        var PointDestination ={
          lat: POIs2[i].lat,
          lng: POIs2[i].lng
        }
        var distance = google.maps.geometry.spherical.computeDistanceBetween(
           new google.maps.LatLng(PointStart),
           new google.maps.LatLng(PointDestination)
        )
        counter++;
      }

      currentMode = 2;
    }
    else if(mode == 3){
      document.getElementById('mode2').classList.remove('mode_btn_active');
      document.getElementById('mode3').classList.add('mode_btn_active');

      for(var i =0; i < 10; i++){
        document.getElementById("R-name"+(i+1)).innerHTML = POIs3[i].name;
        document.getElementById("R-address"+(i+1)).innerHTML = POIs3[i].address;
        document.getElementById("R-pic"+(i+1)).style.backgroundImage = "url('"+POIs3[i].photo+"')";
        document.getElementById("R-dis"+(i+1)).innerHTML = '距離' + POIs3[i].distance + '公尺';
        createRoute(i+1,POIs3[i].lat,POIs3[i].lng);
        directionLa[i+1] = POIs3[i].lat;
        directionLn[i+1] = POIs3[i].lng;
        var PointStart ={
          lat: current_user_lat,
          lng: current_user_lng
        }
        var PointDestination ={
          lat: POIs3[i].lat,
          lng: POIs3[i].lng
        }
        var distance = google.maps.geometry.spherical.computeDistanceBetween(
           new google.maps.LatLng(PointStart),
           new google.maps.LatLng(PointDestination)
        )
      }

      currentMode = 3;
    }
  }
}

function modePrechange(){
  // Mode 3
  if(modePOIs3Switch == 0){
    POIs2.push({
      name: 'blank',
      address: 'blank',
      photo: 'blank',
      lat: 0,
      lng: 0,
      distance: 10000,
      rank: 11
    });
    POIs3.push({
      name: 'blank',
      address: 'blank',
      photo: 'blank',
      lat: 0,
      lng: 0,
      distance: 10000
    });
    modePOIs3Switch = 1;
    for(var i = 10; i >= 0; i--){
      for(var j = 0; j <= i-1; j++){
        if(POIs3[j].distance > POIs3[j+1].distance){
          var tempValue = POIs3[j];
          POIs3[j] = POIs3[j+1];
          POIs3[j+1] = tempValue;
        }
        // Mode 2
        if(POIs2[j].rank > POIs2[j+1].rank){
          var tempValue = POIs2[j];
          POIs2[j] = POIs2[j+1];
          POIs2[j+1] = tempValue;
        }
      }
    }
  }
}
//show direction
function directionshow(rank,temp){
  currentDireciion = rank;
  if(directionSwitch == 0){
    directionSwitch = 1;
  }
  if(directionsDisplay == null){
    directionsDisplay = new google.maps.DirectionsRenderer();
    directionsDisplay.setMap(map);
  }
  if(current_user_lat != 0 && current_user_lng != 0){
    request = {
        origin: { lat: current_user_lat, lng: current_user_lng},
        destination: { lat: directionLa[rank], lng: directionLn[rank] },
        // destination: { lat: 24.94760592, lng: 121.37516264 },
        travelMode: 'WALKING'
      };

    directionsService.route(request, function (result, status) {
        if (status == 'OK') {
            // 回傳路線上每個步驟的細節
            // console.log(result.routes[0].legs[0].steps);
            directionsDisplay.setDirections(result);
        } else {
            console.log(status);
        }
    });
  }
  // else if(!current_user_lat || !current_user_lng){
  //     alert('請先到窗邊或室外接收GPS信號 方能顯示導航路線');
  // }
  // else{
  //   alert('請先到窗邊或室外接收GPS信號 方能顯示導航路線');
  // }
  if(temp == 1){
    if(document.body.clientWidth < 769){
      toggleSidebar();
    }
  }
}

function directionClean(){
  directionSwitch = 0;
  directionsDisplay.setMap(null);
  directionsDisplay = null;
}

function recommend(){
$.getJSON($SCRIPT_ROOT + '/heatpoint_api', {}, function(data){
    var datacount = data[0].length;
    current_user_lat = parseFloat(data[1][0]);
    current_user_lng = parseFloat(data[1][1]);
    for(var i = 0; i < 10; i++){
      // 01
      // 32
      var lngg = data[0][i][0][1] + (data[0][i][1][1] - data[0][i][0][1])/2; //左右
      var latt = data[0][i][3][0] + (data[0][i][0][0] - data[0][i][3][0])/2; //上下
      getPOI(i+1,lngg,latt);
    }
    recommend_name = new Array();
  });
}

function remove(){
  $('#showpic-con').removeClass('showpic-con');
}

function createRoute(rank,latt,lngg){
  var recommend_scene = document.getElementById("R-pic"+rank);
  recommend_scene.addEventListener('click',function(){
    var fenway = {lat : latt, lng : lngg}; 
    var panorama = new google.maps.StreetViewPanorama(
      document.getElementById('pic-container'), {
        position: fenway,
        pov: {
          heading: 34,
          pitch: 10
        }
      });
    map.setStreetView(panorama);
    $('#showpic-con').addClass('showpic-con'); 
  }); 
}
