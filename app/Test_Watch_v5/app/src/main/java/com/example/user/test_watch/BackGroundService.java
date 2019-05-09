package com.example.user.test_watch;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.provider.Settings;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;
import android.support.v4.app.NotificationCompat;

import com.example.user.test_watch.Retrofit.IMyService;
import com.example.user.test_watch.Retrofit.RetrofitClient;
import com.golife.contract.AppContract;
import com.golife.customizeclass.ScanBluetoothDevice;
import com.golife.database.table.TablePulseRecord;
import com.golife.database.table.TableSleepRecord;
import com.golife.database.table.TableSpO2Record;
import com.golife.database.table.TableStepRecord;
import com.goyourlife.gofitsdk.GoFITSdk;
import com.golife.contract.AppContract;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;


import java.util.ArrayList;
import  static com.example.user.test_watch.App.CHANNEL_ID;
import static com.example.user.test_watch.App._goFITSdk;
import  static com.example.user.test_watch.App.isSync;
import  static com.example.user.test_watch.App.Name;
import java.util.Calendar;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;
import retrofit2.Retrofit;

public class BackGroundService extends Service {



    private LocationListener listener;
    private LocationManager locationManager;
    private  String host = "tcp://120.126.136.17:1883";       //改成我們的IP
    private  String userName = Name;                         //讓使用者輸入
    private static String myTopic  = "test";

    private BluetoothAdapter mBluetoothAdapter  = BluetoothAdapter.getDefaultAdapter();
    private static MqttAndroidClient client;
    private MqttConnectOptions conOpt;
    String clientId = MqttClient.generateClientId();


    //x,y 實際位置;lastX,lastY最後位置
    String x = "";
    String y = "";
    String hr= "";
    String o2= "";
    String step= "";
    String lastX="";
    String lastY="";
    long disconnectTime =0;
    boolean notFlag=true;
    private final Handler ServiceHandler = new Handler();

    // Define the code block to be executed
    Runnable ServiceRunnable = new Runnable() {
        @Override
        public void run() {
            // Insert custom code here
            Log.d("GPS", "run Thread    "+isSync);

            SharedPreferences setting_watch =getSharedPreferences("watch_info",MODE_PRIVATE);
            String macAddress = setting_watch.getString("macAddress", "");
            if(mBluetoothAdapter.isEnabled()&&checkNetWork()){

                try{
                    if(isSync==false &&_goFITSdk.isBLEConnect()){

                        //getData();

                        GetHealthData();
                        //紀錄座標
                        if(x!=null&&y!=null){
                            lastX=x;
                            lastY=y;
                        }
                        //broadcast
                        Intent i =new Intent("location_update");
                        i.putExtra("gps_value",lastX + "\n" + lastY);
                        i.putExtra("hr_value",hr);
                        i.putExtra("o2_value",o2);
                        i.putExtra("step_value",step);
                        Log.d("GPS", "location:"+i.getExtras().get("gps_value")+"\nHr:"+i.getExtras().get("hr_value")
                                +"\nO2:"+i.getExtras().get("o2_value")+"\nStep:"+i.getExtras().get("step_value"));
                        sendBroadcast(i);

                        if(!client.isConnected()){
                            init();
                        }
                        pub(hr, o2, step);

                    }else if(!_goFITSdk.isBLEConnect() && macAddress!=""){
                        //斷線
                        Calendar calendar = Calendar.getInstance();
                        long nowTime=calendar.getTimeInMillis();

                        if(disconnectTime==0){
                            disconnectTime=nowTime;
                        }

                        //斷線10分內繼續傳600000
                        if((nowTime-disconnectTime)<600000) {
                            Log.d("GPS", "Start");

                            if(!client.isConnected()){
                                init();
                            }
                            pub("","","");

                            String productID = setting_watch.getString("productID", "");
                            String pairingCode = setting_watch.getString("pairingCode", "");
                            String pairingTime = setting_watch.getString("pairingTime", "");
                            Log.d("GPS", "macAddress: " + macAddress + "-" + productID + "-" + pairingCode + "-" + pairingTime);
                            try {

                                _goFITSdk.doConnectDevice(macAddress, pairingCode, pairingTime, productID, new GoFITSdk.GenericCallback() {
                                    @Override
                                    public void onSuccess() {
                                        Log.d("GPS", "onSuccess: ");
                                        isSync=false;
                                        disconnectTime=0;
                                    }

                                    @Override
                                    public void onFailure(int i, String s) {
                                        Log.d("GPS", "onFailure: " + s);
                                    }
                                });
                            } catch (Exception e) {
                                Log.d("GPS", "E" + e.getMessage());
                            }
                            Log.d("GPS", "BLE connect fail in Thread");
                        }else{
                            //超過10分鐘，停止服務
                            Log.d("GPS", "Destroy service");
                            _goFITSdk.doSendIncomingMessage(AppContract.emIncomingMessageType.Default,"連線逾時","end");
                            _goFITSdk.doDisconnectDevice();

                            Notification notification =new Notification.Builder(BackGroundService.this)
                                    .setContentTitle("GO_LiFE Service")
                                    .setContentText("連線逾時")
                                    .setSmallIcon(R.drawable.ic_android)
                                    .setDefaults(Notification.DEFAULT_ALL)
                                    .build();
                            NotificationManager manager = (NotificationManager)
                                    getSystemService(Context.NOTIFICATION_SERVICE);
                            manager.notify((int)nowTime,notification);

                            Intent i =new Intent(getApplicationContext(),BackGroundService.class);
                            stopService(i);

                        }
                    }

                }catch (Exception e){
                    Log.d("GPS", "Thread Exception"+e.getMessage());
                }

            }else{
                Log.d("GPS", "else:");
                mBluetoothAdapter.enable();
            }

            // Repeat every 10 seconds
            ServiceHandler.postDelayed(this, 10000);
        }
    };


    public void pub(String temHR,String temO2,String temStep){
        Calendar calendar = Calendar.getInstance();
        int year = calendar.get(Calendar.YEAR);
        int month = calendar.get(Calendar.MONTH)+1;
        int day = calendar.get(Calendar.DAY_OF_MONTH);
        int hour = calendar.get(Calendar.HOUR_OF_DAY);
        int minute = calendar.get(Calendar.MINUTE);
        int second = calendar.get(Calendar.SECOND);



        String message = userName+":"+lastY+":"+lastX+":"+temHR+":"+temO2+":"+temStep+":"
                +year+":"+month+":"+day+":" +hour+":"+minute+":"+second;
        Log.d("GPS", "message: "+message);
        try {
            Log.d("GPS", "Connect?  "+client.isConnected());

            client.publish(myTopic, message.getBytes(), 0, false);

            Log.d("GPS", "pub ");
        } catch (Exception e) {
            Log.d("GPS", "MQTTException in publish"+    e.getMessage());
        }
    }
    public void sub(){
        try{
            Log.d("GPS", "sub: ");
            client.subscribe("Warning",0);
        }catch(MqttException e){
            Log.d("GPS", "MQTTException in subscribe");
            e.printStackTrace();
        }
    }
    private void init() {
        String uri = host;
        client = new MqttAndroidClient(this, uri, clientId);
        client.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                String subMessage= message.toString();
                Log.d("GPS", "messageArrived: "+subMessage);
                if(subMessage.equals("HR_HIGH")){
                    Log.d("GPS", "HR_HIGH");
                    _goFITSdk.doSendIncomingMessage(AppContract.emIncomingMessageType.Default,"心律過快!213212121212312312312131213123123","warning");
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        });
        conOpt = new MqttConnectOptions();
        // 清除缓存
        conOpt.setCleanSession(true);
        // 用户名
        conOpt.setUserName(userName);

        doClientConnection();

    }

    private void doClientConnection() {
        final Handler handler = new Handler();
        if (!client.isConnected()) {
            try {
                IMqttToken token=client.connect(conOpt);
                token.setActionCallback(new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        sub();
                        handler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(BackGroundService.this.getApplicationContext(),"MQTT connected!",Toast.LENGTH_SHORT).show();
                            }
                        });
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        handler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(BackGroundService.this.getApplicationContext(),"MQTT connection failed!",Toast.LENGTH_SHORT).show();
                            }
                        });
                        exception.printStackTrace();
                    }
                });
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    public void GetHealthData(){
        /*
        final TextView hr_value = (TextView) findViewById(R.id.hr_value);
        final TextView o2_value = (TextView) findViewById(R.id.o2_value);
        final TextView step_value = (TextView) findViewById(R.id.step_value);
        final TextView gps_value = (TextView) findViewById(R.id.gps_value);
        */
        final Handler ToastHandler = new Handler();
        final String _tag = "GPS";
        Log.d("GPS", "GetHealthData: ");

        if(_goFITSdk==null){
            Log.d("GPS", "SDK null?: NULL"+_goFITSdk);
        }else{
            Log.d("GPS", "SDK null?: Exist"+_goFITSdk);
        }

        if(_goFITSdk.isBLEConnect()){
            Log.d(_tag, "GetHealthData: Connected"+_goFITSdk.isBLEConnect());
        }
        else{
            Log.d(_tag, "GetHealthData: Disconnected"+_goFITSdk.isBLEConnect());
            return;
        }

        _goFITSdk.doSyncFitnessData(new GoFITSdk.SyncCallback() {
            @Override
            public void onCompletion() {
                isSync = false;
                /*
                ToastHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(BackGroundService.this.getApplicationContext(), "Sync complete!", Toast.LENGTH_SHORT).show();
                    }
                });
                */

                //clear data
                _goFITSdk.doClearDeviceData(new GoFITSdk.GenericCallback() {
                    @Override
                    public void onSuccess() {
                        /*
                        ToastHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(BackGroundService.this.getApplicationContext(), "Success Clear Data", Toast.LENGTH_SHORT).show();
                            }
                        });
                        */
                    }

                    @Override
                    public void onFailure(int i, String s) {
                        Log.i(_tag, "clear data onFailure: " + s);
                    }
                });
            }

            @Override
            public void onProgress(String s, int i) {
                isSync = true;
                Log.i(_tag, "doSyncFitnessData() : onProgress() : message = " + s + ", progress = " + i);
            }

            @Override
            public void onGetFitnessData(ArrayList<TableStepRecord> stepRecords, ArrayList<TableSleepRecord> sleepRecords, ArrayList<TablePulseRecord> hrRecords, ArrayList<TableSpO2Record> spo2Records) {
                Log.d("SIZE", ""+stepRecords.size()+"_"+hrRecords.size()+"_"+spo2Records.size());
                for(int i = 0; i < stepRecords.size(); i++){
                    Log.d("A", i+" step = " + stepRecords.get(i).getSteps() + "  timestamp : " + stepRecords.get(i).getTimestamp());
                    if(i == (stepRecords.size()-1)){
                        Log.d(_tag, "Current Step: " + stepRecords.get(i).getSteps());
                        step=Integer.toString(stepRecords.get(i).getSteps());
                        //step_value.setText(Integer.toString(stepRecords.get(i).getSteps()));
                    }
                }

                Log.d("87:", "sleep: start:" +sleepRecords.size());
                for (int i = 0; i < sleepRecords.size(); i++) {
                    Log.d("87:", "sleep: " + sleepRecords.get(i).getScore());
                }

                for(int i = 0; i < hrRecords.size(); i++){

                    Log.d("A", i+" hr = " + hrRecords.get(i).getPulse() + "  timestamp : " + hrRecords.get(i).getTimestamp());
                    if(i == (hrRecords.size()-1)){
                        Log.d(_tag, "Current HR: " + hrRecords.get(i).getPulse());
                        hr=Integer.toString(hrRecords.get(i).getPulse());
                        //hr_value.setText(Integer.toString(hrRecords.get(i).getPulse()));
                    }
                }

                for(int i = 0; i < spo2Records.size(); i++){
                    Log.d("A", i+" o2 = " + spo2Records.get(i).getSpO2() + "  timestamp : " + spo2Records.get(i).getTimestamp());
                    if(i == (spo2Records.size()-1)){
                        Log.d(_tag, "Current o2: " + spo2Records.get(i).getSpO2());
                        o2=Integer.toString(spo2Records.get(i).getSpO2());
                        //o2_value.setText(Integer.toString(spo2Records.get(i).getSpO2()));
                    }
                }
            }

            @Override
            public void onFailure(int i, String s) {
                Log.i(_tag, "onFailure: " + s);
                Log.i(_tag, "onFailure: " + i);
            }
        });
    }

    public void getData(){
        CompositeDisposable compositeDisposable = new CompositeDisposable();
        IMyService iMyService;

        Retrofit retrofitClient = RetrofitClient.getInstance();
        iMyService = retrofitClient.create(IMyService.class);

        String n="james";
        int y=2019,m=5,d=6;
        compositeDisposable.add(iMyService.getDayData(n,y,m,d)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Consumer<String>() {
                    @Override
                    public void accept(String response) throws Exception {
                        _goFITSdk.doSendIncomingMessage(AppContract.emIncomingMessageType.Default,"今日目標    "+response+"步","getDay");
                        Log.d("test", response);
                    }
                }));
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent){
        return  null;
    }

    @SuppressLint("MissingPermission")
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.d("GPS", "onStartCommand");
        init();

        listener = new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                /*
                Intent i =new Intent("location_update");
                i.putExtra("coordinates",location.getLatitude() + " " + location.getLongitude());

                sendBroadcast(i);

                Log.d("GPS", "test:" +i.getExtras().get("coordinates"));
                */
                x = String.valueOf(location.getLatitude());
                y = String.valueOf(location.getLongitude());

            }

            @Override
            public void onStatusChanged(String provider, int status, Bundle extras) {

            }

            @Override
            public void onProviderEnabled(String provider) {

            }

            @Override
            public void onProviderDisabled(String provider) {
                Intent i =new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(i);
            }
        };
        locationManager = (LocationManager)getApplicationContext().getSystemService(Context.LOCATION_SERVICE);

        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,500,0,listener);

        ServiceHandler.post(ServiceRunnable);

        Intent notificationIntent=new Intent(this,Main2Activity.class);
        PendingIntent pendingIntent=PendingIntent.getActivity(this,
                0,notificationIntent,0);
        Notification notification =new NotificationCompat.Builder(this,CHANNEL_ID)
                .setContentTitle("GO_LiFE Service")
                .setContentText("Service is running!!!")
                .setSmallIcon(R.drawable.ic_android)
                .setContentIntent(pendingIntent)
                .build();

        startForeground(1, notification);

        return START_NOT_STICKY;
    }

    private boolean checkNetWork() {
        ConnectivityManager mConnectivityManager =
                (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = mConnectivityManager.getActiveNetworkInfo();

        if (mNetworkInfo ==null){
            Intent callNetSettingIntent = new Intent(
                    Settings.ACTION_SETTINGS);
            Toast.makeText(BackGroundService.this, "請前往開啟網路", Toast.LENGTH_LONG).show();
            callNetSettingIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(callNetSettingIntent);
            return false;
        }else{
            return true;
        }

    }

    @SuppressLint("MissingPermission")
    @Override
    public void onCreate(){
        super.onCreate();
        Log.d("GPS", "onCreate: ");

    }



    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d("GPS", "onDestroy: ");
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
        ServiceHandler.removeCallbacks(ServiceRunnable);
        if(locationManager != null){
            locationManager.removeUpdates(listener);
        }
    }
}
