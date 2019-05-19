var myLineChart1,myLineChart2,myLineChart3,myLineChart4;

function getdate(){
    var day = document.getElementById('date');
    var date = day.value;
    return date
}

//week: ture  day: false
function button(){
    var date = getdate();
	var toggle_hr = document.getElementById('switch-container-hr');
    var toggleContainer_hr = document.getElementById('switch-toggle-container-hr');
    var toggleNumber_hr;
    toggle_hr.addEventListener('click', function() {
        toggleNumber_hr = !toggleNumber_hr;
        if (toggleNumber_hr) {
            toggleContainer_hr.style.WebkitClipPath = 'inset(0 0 0 50%)';
            toggleContainer_hr.style.backgroundColor = '#D74046';
        } else {
            toggleContainer_hr.style.WebkitClipPath = 'inset(0 50% 0 0)';
            toggleContainer_hr.style.backgroundColor = 'dodgerblue';
        }
        console.log(toggleNumber_hr);
        getdata("hr_value", !toggleNumber_hr, date);
    });


    // var toggle_ox = document.getElementById('switch-container-ox');
    // var toggleContainer_ox = document.getElementById('switch-toggle-container-ox');
    // var toggleNumber_ox;   //0 day 1 week
    // toggle_ox.addEventListener('click', function() {
    //     toggleNumber_ox = !toggleNumber_ox;
    //     if (toggleNumber_ox) {
    //         toggleContainer_ox.style.clipPath = 'inset(0 0 0 50%)';
    //         toggleContainer_ox.style.backgroundColor = '#D74046';
    //     } else {
    //         toggleContainer_ox.style.clipPath = 'inset(0 50% 0 0)';
    //         toggleContainer_ox.style.backgroundColor = 'dodgerblue';
    //     }
    //     console.log(toggleNumber_ox);
    //     getdata("o2_value", !toggleNumber_ox, date);
    // });

    // var toggle_st = document.getElementById('switch-container-st');
    // var toggleContainer_st = document.getElementById('switch-toggle-container-st');
    // var toggleNumber_st;   //0 day 1 week
    // toggle_st.addEventListener('click', function() {
    //     toggleNumber_st = !toggleNumber_st;
    //     if (toggleNumber_st) {
    //         toggleContainer_st.style.clipPath = 'inset(0 0 0 50%)';
    //         toggleContainer_st.style.backgroundColor = '#D74046';
    //     } else {
    //         toggleContainer_st.style.clipPath = 'inset(0 50% 0 0)';
    //         toggleContainer_st.style.backgroundColor = 'dodgerblue';
    //     }
    //     console.log(toggleNumber_st);
    //     getdata("step_value", !toggleNumber_st, date);
    // });
}


function SetChart_hr(x_time,y_data){
	var ctx_hr = document.getElementById('myChart_HR');
    var yAxis = x_time;
    var xAxis = y_data;

    myLineChart1 = new Chart(ctx_hr, {
        type: 'line',
        data: {
            labels:yAxis,
            datasets:[{
                label: '',
                data: xAxis,
                backgroundColor: "rgba(0,148,255,0.6)",
            }]
        },
        options: {
            legend: {
                display: false
            }
        },
    });
}

function SetChart_ox(x_time,y_data){
    var ctx_ox = document.getElementById('myChart_OX');
    var yAxis = x_time;
    var xAxis = y_data;

    myLineChart2 = new Chart(ctx_ox, {
        type: 'line',
        data: {
            labels:yAxis,
            datasets:[{
                label:'血氧',
                data: xAxis,
                backgroundColor: "rgba(255,0,0,0.6)"
            }]
        },
        options: {
            legend: {
                display: false
            }
        },
    });
    myLineChart2.update();
        
}

function SetChart_step(x_time,y_data){
    var ctx_st = document.getElementById('myChart_ST');
    var yAxis = x_time;
    var xAxis = y_data;

    myLineChart3 = new Chart(ctx_st, {
        type: 'line',
        data: {
            labels:yAxis,
            datasets:[{
                label:'步數',
                data: xAxis,
                backgroundColor: "rgba(107,142,35,0.6)"
            }]
        },
        options: {
            legend: {
                display: false
            }
        },
    });
}

function Set_comparisonChart(x,p_y,r_y){
    var ctx_st = document.getElementById('myChart_comparison');

    myLineChart4 = new Chart(ctx_st, {
        type: 'line',
        data: {
            labels:x,
            datasets:[{
                label:'預測步數',
                data: p_y,
                backgroundColor: "rgba(52, 119, 226,0)",
                borderColor: "rgba(52, 119, 226,0.6)"
            },
            {
                label:'實際步數',
                data: r_y,
                backgroundColor: "rgba(226, 67, 49,0)",
                borderColor: "rgba(226, 67, 49,0.6)"
            }
            ]
        },
        options: {
        },
    });
}

function SetChart_comparison(time){
  $.getJSON($SCRIPT_ROOT + '/prediction_api', {
            time : time
            }, function(data){
                Set_comparisonChart(data[0],data[1],data[2])
            });
}

function getdata(type, duration, time){
	$.getJSON($SCRIPT_ROOT + '/GetHealth_api', {
            type : type,
            duration : duration,
            time : time
            }, function(data){
                if (type == 'hr_value'){ SetChart_hr(data[0],data[1]); }
                else if (type == 'o2_value'){ SetChart_ox(data[0],data[1]); }
                else { SetChart_step(data[0],data[1]); }
            });
}

function change_person(person){

}

function showall(){
    date = getdate();
    getdata("hr_value", 0, date);
    getdata("o2_value", 0, date);
    getdata("step_value", 0, date);
}
function chartShowHr(){
    document.getElementById('health-btn-st').classList.remove('current-health-btn');
    document.getElementById('health-btn-ox').classList.remove('current-health-btn');
    document.getElementById("chart-box-ox").classList.add('chart-box-hidden');
    document.getElementById("chart-box-st").classList.add('chart-box-hidden');
    document.getElementById("chart-box-hr").classList.remove('chart-box-hidden');
    document.getElementById('health-btn-hr').classList.add('current-health-btn');
}
function chartShowOx(){
    document.getElementById('health-btn-hr').classList.remove('current-health-btn');
    document.getElementById('health-btn-st').classList.remove('current-health-btn');
    document.getElementById("chart-box-hr").classList.add('chart-box-hidden');
    document.getElementById("chart-box-st").classList.add('chart-box-hidden');
    document.getElementById("chart-box-ox").classList.remove('chart-box-hidden');
    document.getElementById('health-btn-ox').classList.add('current-health-btn');
}
function chartShowSt(){
    document.getElementById('health-btn-hr').classList.remove('current-health-btn');
    document.getElementById('health-btn-ox').classList.remove('current-health-btn');
    document.getElementById("chart-box-hr").classList.add('chart-box-hidden');
    document.getElementById("chart-box-ox").classList.add('chart-box-hidden');
    document.getElementById("chart-box-st").classList.remove('chart-box-hidden');
    document.getElementById('health-btn-st').classList.add('current-health-btn');
}


var map,heatmap,polyline,area,setarea,set_ana,setEF,setEF1,marker,temp1 = 0, temp2 = 0, fenceInitialize = 0,
    spacelng = 0, valuelng = 0, filenumber = 0, heatmapMode = 1;
    // fenceInitialize = temp3 =>> whether the fence have been draw before
    var numbers = '12345678910';
    var counter = 0;
    var polys = [], p;
    var point_array = [];
    var EF_array =[];
    var EF_array1 = [];
    var number_array = [];
    var Global_color = '#000000';
    var Global_spacelist = [];
    var Global_valuelist = [];
    var Global_fenceScale = 10;
    var Global_centerlat;
    var Global_centerlng;
    var Global_date = [];
    var HeatMapData = [];
    function initialize(){
      var myLatlng = new google.maps.LatLng(24.944, 121.3695);
      var mapOptions = {
        maxZoom: 16,
        zoom: 16,
        center: myLatlng,
        zoomControl: true,
        scaleControl: false,
        rotateControl: false,
        mapTypeControl: false,
        streetViewControl: false,
        fullscreenControl: false,
        mapTypeId: google.maps.MapTypeId.ROADMAP
      }

      if(getCookie('StartDate')){
        console.log("get cookie");
        Global_date[0] = getCookie('StartDate');
        Global_date[1] = getCookie('EndDate');
      }
      else{
        console.log("get date");
        Global_date[0] = new Date('2018.01.01').getTime();
        Global_date[1] = new Date().getTime();
      }
      map = new google.maps.Map(document.getElementById('map_canvas'), mapOptions);
      // map.addListener('idle', function() {
      //   var c = map.getCenter();
      //   Global_centerlat = map.getCenter().lat(); 
      //   Global_centerlng = map.getCenter().lng();
      // });
      $.getJSON($SCRIPT_ROOT + '/change_Date', {
          startDate : Global_date[0],
          endDate : Global_date[1],
          fenceScale : Global_fenceScale
            // send lat, lng through JQuery
          }, function(data){
            console.log('move change');
            if(getCookie('FenceColor'))
            {
              Global_color = getCookie('FenceColor');
            }
            // flask send back data through JQuery
            if(fenceInitialize == 0){
              Global_spacelist = data[0];
              Global_valuelist = data[1];
              set_EF(data[0],data[1],Global_color);
              console.log('leo');
              fenceInitialize = 1;
            }
            else if(fenceInitialize == 1){
              clear();
              Global_spacelist = data[0];
              Global_valuelist = data[1];
              set_EF(data[0],data[1],Global_color);
            }
          });


        // If the fenceScale change change cookie
        // var optionListener = document.getElementById("FenceScaleSelect");
        // optionListener.addEventListener("change", function() {

        //   var option = optionListener.value;
        //   setCookie('FenceScale', option);
        //   Global_fenceScale = option;
        //   $.getJSON($SCRIPT_ROOT + '/change_Date', {
        //     startDate : Global_date[0],
        //     endDate : Global_date[1],
        //     lat : Global_centerlat,
        //     lng : Global_centerlng,
        //     fenceScale : Global_fenceScale
        //         // send lat, lng through JQuery
        //       }, function(data){
        //         if(getCookie('FenceColor'))
        //         {
        //           Global_color = getCookie('FenceColor');
        //         }
        //         // flask send back data through JQuery
        //         if(fenceInitialize == 0){
        //           Global_spacelist = data[0];
        //           Global_valuelist = data[1];
        //           set_EF(data[0],data[1],Global_color);
        //           fenceInitialize = 1;
        //         }
        //         else if(fenceInitialize == 1){
        //           clear();
        //           Global_spacelist = data[0];
        //           Global_valuelist = data[1];
        //           set_EF(data[0],data[1],Global_color);
        //           fenceInitialize = 1;
        //         }
        //       });

        //   var fenceScale;
        //   if(getCookie('FenceScale'))
        //   {
        //     fenceScale = getCookie('FenceScale');
        //   }
        //   else
        //   {
        //     fenceScale = 10;
        //   }
        //   Global_fenceScale = fenceScale;
        //   $('#FenceScaleDefault').text(fenceScale);
        // });

        var fenceScale;
        if(getCookie('FenceScale'))
        {
          fenceScale = getCookie('FenceScale');
        }
        else
        {
          fenceScale = 10;
        }
        Global_fenceScale = fenceScale;
        $('#FenceScaleDefault').text(fenceScale);
        // Set FenceScale to be the cookie

        $(function() {
          var startd, endd;
          if(getCookie('StartDate')){
            startd = getCookie('StartDate');
            endd = getCookie('EndDate');
          }
          else{
            startd = new Date('2018-01-01').getTime();
            endd = new Date().getTime();
          }
          $( "#slider-range" ).slider({
            range: true,
            min: new Date('2018-01-01').getTime(),
            max: new Date().getTime(),
              step: 86400, // every setp is one day
              values: [ startd, endd],
              slide: function( event, ui ) {
                $( "#amount" ).val( (new Date(ui.values[ 0 ]).toDateString() ) + " - " + (new Date(ui.values[ 1 ])).toDateString() );
                
                if (Global_date.length == 0)
                {
                  Global_date.push(ui.values[0]);
                  Global_date.push(ui.values[1]);
                } 
                else
                {
                  Global_date[0] = ui.values[0];
                  Global_date[1] = ui.values[1];
                }
              }

            });
            //initial presentation
            if(getCookie('StartDate')){
              $( "#amount" ).val(new Date(parseInt(getCookie('StartDate'))).toDateString() +
                " - " + (new Date(parseInt(getCookie('EndDate'))).toDateString()));
            }
            else{
              $( "#amount" ).val( (new Date($( "#slider-range" ).slider( "values", 0 )).toDateString()) +
                " - " + (new Date($( "#slider-range" ).slider( "values", 1 ))).toDateString());
            }
            
          });

        document.getElementById("submitDate").addEventListener("click", function () {
          console.log(Global_date[0]);
          console.log(Global_date[1]);
          $.getJSON($SCRIPT_ROOT + '/change_Date', {
            startDate : Global_date[0],
            endDate : Global_date[1],
            lat : Global_centerlat,
            lng : Global_centerlng,
            fenceScale : Global_fenceScale
              //   // send lat, lng through JQuery
            }, function(data){
              console.log('change_Date');
              if(getCookie('FenceColor'))
              {
                Global_color = getCookie('FenceColor');
              }
                // flask send back data through JQuery
                setCookie('StartDate',Global_date[0]);
                setCookie('EndDate',Global_date[1]);
                if(fenceInitialize == 0){
                  Global_spacelist = data[0];
                  Global_valuelist = data[1];
                  set_EF(data[0],data[1],Global_color);
                  fenceInitialize = 1;
                }
                else if(fenceInitialize == 1){
                  clear();
                  console.log(data[0]);
                  console.log(data[1]);
                  Global_spacelist = data[0];
                  Global_valuelist = data[1];
                  set_EF(data[0],data[1],Global_color);
                }
              }); 
        });


      }

      function setCookie(name, value)
      {
        document.cookie=name+"="+escape(value)+"; path=/;";
      }

      function getCookie(cname) {
        var name = cname + "=";
        var decodedCookie = decodeURIComponent(document.cookie);
        var ca = decodedCookie.split(';');
        for(var i = 0; i <ca.length; i++) {
          var c = ca[i];
          while (c.charAt(0) == ' ') {
            c = c.substring(1);
          }
          if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
          }
        }
        return "";
      }

      // function setline(){
      //   var points= [];
      //   {% for data in datas %}

      //   var temp = {lat: {{data.latitude}}, lng: {{data.longitude}}};
      //   points.push(temp);
      //   {% endfor %}
      //   console.log(points);
      //   if(!polyline){
      //     polyline = new google.maps.Polyline({
      //       path: points,
      //       strokeColor: '#FF0000',
      //       strokeOpacity: 0.7,
      //       strokeWeight:  5
      //     });
      //   }
      //   polyline.setMap(map);
      // }

      function remove_line(){
        polyline.setMap(null);
      }

      function set_area(points){
        if(temp2 == 0){
          var w = 0.00005;
          var h = w;

          for(var i = 0; i < points.length; i++){
            var fence = [
            {lat: points[i][0] + h, lng: points[i][1] - w},
            {lat: points[i][0] + h, lng: points[i][1] + w},
            {lat: points[i][0] - h, lng: points[i][1] + w},
            {lat: points[i][0] - h, lng: points[i][1] - w}
            ];

            setarea = new google.maps.Polygon({
              position: {lat: points[i][0], lng: points[i][1]},
              path: fence,
              strokeColor: '#FF0000',  
              strokeOpacity: 0,  
              strokeWeight: 1,  
              fillColor: '#FF0000',  
              fillOpacity: 0.4
            });
            point_array.push(setarea);
            setarea.setMap(map);
          }
          temp2 = 1;
        }
        else{
          for(var i = 0; i < points.length; i++){
            point_array[i].setMap(null);
          }
          point_array = [];
          temp2 = 0;
        }
      }

      function set_EF(spacelist,valuelist,color){
        var SO = 1; FO = 0.2;
        if(heatmapMode == 1){
          SO = 0;
          FO = 0;
        }

        spacelng = spacelist.length;
        for(var i = 0; i < spacelist.length; i++){
          var fence = [
          // draw the rectangular four point
          {lat: spacelist[i][0][0], lng: spacelist[i][0][1]},
          {lat: spacelist[i][1][0], lng: spacelist[i][1][1]},
          {lat: spacelist[i][2][0], lng: spacelist[i][2][1]},
          {lat: spacelist[i][3][0], lng: spacelist[i][3][1]}
          ]; 

          setEF = new google.maps.Polygon({
            position: {lat: spacelist[i][0][0], lng: spacelist[i][0][1]},
            path: fence,
            strokeColor: '#000000',  
            strokeOpacity: SO,  
            strokeWeight: 1,  
            fillColor: '#000',  
            fillOpacity: 0
          });
          EF_array.push(setEF);
          setEF.setMap(map);
        }
        valuelng = valuelist.length;
        for(var i = 0; i < valuelist.length; i++){
          var fence = [
          // draw the rectangular four point
          {lat: valuelist[i][0][0], lng: valuelist[i][0][1]},
          {lat: valuelist[i][1][0], lng: valuelist[i][1][1]},
          {lat: valuelist[i][2][0], lng: valuelist[i][2][1]},
          {lat: valuelist[i][3][0], lng: valuelist[i][3][1]}
          ]; 

          var c = valuelist[i][4];
          // the scale from the value list
          setEF1 = new google.maps.Polygon({
            position: {lat: valuelist[i][0][0], lng: valuelist[i][0][1]},
            path: fence,
            strokeColor: '#000000',  
            strokeOpacity: SO,  
            strokeWeight: 1,  
            fillColor: Global_color,  
            // fillOpacity: 0.1*c
            fillOpacity: FO*c
          });

          if(i < 10){
            num = numbers[i];
            if(i == 9){
              num = '10';
            }
            var latt = valuelist[i][0][0] + (valuelist[i][3][0] - valuelist[i][0][0])/2;
            var lngg = valuelist[i][0][1] + (valuelist[i][1][1] - valuelist[i][0][1])/2;

            var HeatMapLatLng = new google.maps.LatLng(latt,lngg);
            HeatMapData.push(HeatMapLatLng);

            keypoint = 
            {lat: latt, lng: lngg};
            marker = new google.maps.Marker({
              position: keypoint,
              label: num
            });
            // marker.setMap(map);
            number_array.push(marker);

            createRoute(setEF1,counter,valuelist);

            google.maps.event.addListener(setEF1,'mouseover',function(){
              this.setOptions({strokeColor: '#000000',strokeOpacity: 1, strokeWeight:2});
            }); 
            google.maps.event.addListener(setEF1,'mouseout',function(){
             this.setOptions({strokeOpacity: 0});
           }); 
          }

          EF_array1.push(setEF1);
          setEF1.setMap(map);
          counter++;
        }
        heatmap = new google.maps.visualization.HeatmapLayer({
          data: HeatMapData,
          radius: 60,
          opacity: 0.5
        });
        if(heatmapMode == 1){
          heatmap.setMap(map);
        }
      }

      function clear(){
        for(var i = 0; i < spacelng; i++){
            EF_array[i].setMap(null);
        }
        for(var i = 0; i < valuelng; i++){
            EF_array1[i].setMap(null);
            if(!heatmap){

            }
            else{
              heatmap.setMap(null);
            }
        }
        for(var i = 0; i < 10; i++){
          if(number_array[i]){
            number_array[i].setMap(null);
          }
        }
        number_array = [];
        EF_array = [];
        EF_array1 = [];
        HeatMapData =[];
        counter = 0;
      }
      function setTextColor(picker) {
        var color = '#' + picker.toString();
        if(fenceInitialize== 1){
          clear();
          Global_color = color;
          setCookie('FenceColor',Global_color)
          set_EF(Global_spacelist,Global_valuelist,Global_color);
        }
        else if(fenceInitialize == 0){
          Global_color = color;
          setCookie('FenceColor',Global_color)
          set_EF(Global_spacelist,Global_valuelist,Global_color);
          fenceInitialize=1;
        }
      }

      function remove(){
        $('#showpic-con').addClass('notshowpic-con');
      }

      function createRoute(marker, routenum, valuelist){
        google.maps.event.addListener(marker,'click',function(){
          var fenway = {lat : valuelist[routenum][0][0] - 0.0005,lng : valuelist[routenum][0][1] + 0.0005}; 
          var panorama = new google.maps.StreetViewPanorama(
            document.getElementById('pic-container'), {
              position: fenway,
              pov: {
                heading: 34,
                pitch: 10
              }
            });
          map.setStreetView(panorama);
          $('#showpic-con').removeClass('notshowpic-con'); 
        }); 
      }

      function OpenOrCloseFence(){
        if(fenceInitialize == 0){
          console.log("hi");
          set_EF(Global_spacelist,Global_valuelist,Global_color);            
          fenceInitialize = 1;
        }
        else{
          clear();
          fenceInitialize=0;
        }
      }

      function toggleSidebar(){
        document.getElementById("sidebar").classList.toggle('not-visible');
        document.getElementById("caret-left").classList.toggle('caret-right');
        document.getElementById("slider-con").classList.toggle('active');
      }

      $(function () {
        initialize(null);
        // toggleSidebar();
      // set_EF({{ spacelist }},{{valuelist}},'#000000');
    });

      function map_menu_toggle(){
        document.getElementById("map-menu-drop").classList.toggle('map-menu-close');
      }

      function map_menu_change_heat(){
        clear();
        heatmapMode = 0;
        set_EF(Global_spacelist,Global_valuelist,Global_color);
        document.getElementById("drop-grid").classList.add('map-menu-change');
        document.getElementById("drop-heat").classList.remove('map-menu-change');
        document.getElementById("color-change").classList.remove('map-menu-change');
        document.getElementById("scale-change").classList.remove('map-menu-change');
      }
      function map_menu_change_grid(){
        clear();
        heatmapMode = 1;
        set_EF(Global_spacelist,Global_valuelist,Global_color);
        document.getElementById("drop-grid").classList.remove('map-menu-change');
        document.getElementById("drop-heat").classList.add('map-menu-change');
        document.getElementById("color-change").classList.add('map-menu-change');
        document.getElementById("scale-change").classList.add('map-menu-change');
      }
      function nav_people_show(){
        document.getElementById("nav-people").classList.toggle('nav-menu-hidden');
      }
      function nav_menu_show(){
        document.getElementById("nav-menu").classList.toggle('nav-menu-hidden');
      }

