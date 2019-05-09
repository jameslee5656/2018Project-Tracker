package com.example.user.test_watch;

import android.Manifest;
import android.app.Dialog;
import android.app.TimePickerDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.provider.ContactsContract;
import android.support.annotation.NonNull;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;

import com.example.user.test_watch.Retrofit.IMyService;
import com.example.user.test_watch.Retrofit.RetrofitClient;
import com.golife.contract.AppContract;
import com.golife.customizeclass.CareAlarm;
import com.golife.customizeclass.CareMeasureHR;
import com.golife.customizeclass.ScanBluetoothDevice;
import com.golife.customizeclass.SetCareSetting;
import com.golife.database.table.TablePulseRecord;
import com.golife.database.table.TableSleepRecord;
import com.golife.database.table.TableSpO2Record;
import com.golife.database.table.TableStepRecord;
import com.goyourlife.gofitsdk.GoFITSdk;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;
import java.util.stream.Collectors;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;
import retrofit2.Retrofit;

import  static com.example.user.test_watch.App._goFITSdk;
import  static com.example.user.test_watch.App.isSync;


public class Main2Activity extends AppCompatActivity {

    //private static GoFITSdk _goFITSdk = null;
    private String sdk_license = null;
    private String sdk_certificate = null;
    private ScanBluetoothDevice ScanResult;
    //private Boolean isSync = true;
    private BroadcastReceiver broadcastReceiver;
    private TextView hr_value;
    private TextView o2_value;
    private TextView step_value;
    private TextView gps_value;
    private ImageButton start_service;
    private ImageButton stop_service ;

    //private TimePickerDialog timePickerDialog;

    private BluetoothAdapter mBluetoothAdapter  = BluetoothAdapter.getDefaultAdapter();
    private String mac = null;
    private CheckBox cb;
    @Override
    protected void onResume() {
        super.onResume();
        if(broadcastReceiver == null){
            broadcastReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    hr_value.setText(""+intent.getExtras().get("hr_value"));
                    o2_value.setText(""+intent.getExtras().get("o2_value"));
                    step_value.setText(""+intent.getExtras().get("step_value"));
                    gps_value.setText(""+intent.getExtras().get("gps_value"));

                    Log.d("MAIN", "onReceive:"+intent.getExtras().get("gps_value")+"\n"+intent.getExtras().get("hr_value"));
                    //textView.append("\n" +intent.getExtras().get("coordinates"));
                    //Log.d("GPS_MAIN", "test:" +intent.getExtras().get("coordinates"));
                }
            };
        }
        registerReceiver(broadcastReceiver,new IntentFilter("location_update"));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(broadcastReceiver != null){
            unregisterReceiver(broadcastReceiver);
        }
    }

    private void enable_buttons() {

        Log.d("MAIN", "enable_buttons: ");
        start_service.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                SharedPreferences setting_watch =getSharedPreferences("watch_info",MODE_PRIVATE);
                String macAddress = setting_watch.getString("macAddress", "");

                Log.d("MAIN", "onClick: "+macAddress);
                //第一次連線
                if(!cb.isChecked()){
                    //清空
                    setting_watch.edit().putString("macAddress","")
                                        .putString("productID","")
                                        .putString("pairingCode","")
                                        .putString("pairingTime","")
                                        .commit();
                    scan(v);
                }else{
                    //先前已記住mac
                    Log.d("MAIN", "onClick2: "+macAddress);
                    if(macAddress=="") {
                        Log.d("MAIN", "onClick3: "+macAddress);
                        Toast.makeText(Main2Activity.this,"您先前尚未配對任何連線",Toast.LENGTH_SHORT).show();
                        return;
                    }

                }

                Intent i = new Intent(getApplicationContext(), BackGroundService.class);
                stopService(i);
                ContextCompat.startForegroundService(getApplicationContext(), i);
            }
        });


        stop_service.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i =new Intent(getApplicationContext(),BackGroundService.class);
                stopService(i);
                cancel(v);

            }
        });
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if(requestCode == 100){
            if( grantResults[0] == PackageManager.PERMISSION_GRANTED && grantResults[1] == PackageManager.PERMISSION_GRANTED){
                enable_buttons();
            }else {
                runtime_permissions();
            }
        }
    }
    private boolean runtime_permissions() {
        if(Build.VERSION.SDK_INT >= 23 && ContextCompat.checkSelfPermission(this, Manifest.permission
                .ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ContextCompat
                .checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED){

            requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION},100);

            return true;
        }
        return false;
    }

    /*
    public void setTime(View v){
        timePickerDialog.show();
    }*/

    @RequiresApi(api = Build.VERSION_CODES.N)
    public void getData(View v){
        CompositeDisposable compositeDisposable = new CompositeDisposable();
        IMyService iMyService;

        Retrofit retrofitClient = RetrofitClient.getInstance();
        iMyService = retrofitClient.create(IMyService.class);

        String n="james";
        Date date = new Date();
        Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("Asia/Taipei"));
        cal.setTime(date);
        int y = cal.get(Calendar.YEAR);
        int m = cal.get(Calendar.MONTH) + 1;
        int d = cal.get(Calendar.DAY_OF_MONTH);
        int h = cal.get(Calendar.HOUR_OF_DAY);
        String concat = Integer.toString(y) + Integer.toString(m) + Integer.toString(d) + Integer.toString(h);
        Toast.makeText(Main2Activity.this,concat, Toast.LENGTH_SHORT).show();
        compositeDisposable.add(iMyService.getRank(n,y,m,d,h)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Consumer<String>() {
                    @Override
                    public void accept(String response) throws Exception {
//                        _goFITSdk.doSendIncomingMessage(AppContract.emIncomingMessageType.GMail,"今日目標    "+response+"步","getDay");
//                        Toast.makeText(Main2Activity.this, response, Toast.LENGTH_SHORT).show();
                        Log.i("debug", response);
                    }
                }));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main2);
        hr_value = (TextView) findViewById(R.id.hr_value);
        o2_value = (TextView) findViewById(R.id.o2_value);
        step_value = (TextView) findViewById(R.id.step_value);
        gps_value = (TextView) findViewById(R.id.gps_value);
        start_service = (ImageButton)findViewById(R.id.start_service);
        stop_service = (ImageButton)findViewById(R.id.stop_service);
        final TextView watchstatus = (TextView) findViewById(R.id.watchstatus);
        cb =(CheckBox)findViewById(R.id.checkBox);

        /*
        GregorianCalendar calendar=new GregorianCalendar();
        timePickerDialog=new TimePickerDialog(this, new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int hourOfDay, int minute) {

            }
        },calendar.get(Calendar.HOUR_OF_DAY),calendar.get(Calendar.MINUTE),false);
    */


        //開藍芽
        if(!mBluetoothAdapter.isEnabled()){
            mBluetoothAdapter.enable();
        }
        // Read certificate from file
        sdk_certificate = null;
        final String _tag = "get sdk_certificate";
        try {
            InputStream inputstream = this.getAssets().open("client_cert_2de636ddf2b946e285c5a40528974b6e.crt");
            BufferedReader reader = new BufferedReader(new InputStreamReader(inputstream));
            StringBuilder sb = new StringBuilder();
            String line = null;
            while ((line = reader.readLine()) != null) {
                sb.append(line).append("\n");
            }
            reader.close();
            sdk_certificate = sb.toString();
        }
        catch (Exception e) {
            Log.e(_tag, e.toString());
            Toast.makeText(this, "Exception : " + e.toString(), Toast.LENGTH_SHORT).show();
        }

        //SDK initialize
        if(_goFITSdk==null) {
            this.initSDK();
        }

        if(!runtime_permissions()){
            Log.d("MAIN", "t");
            enable_buttons();
        }


        final Handler handler = new Handler();
        Runnable runnable = new Runnable() {

            @Override
            public void run() {
                Log.d("MAIN", "run: ");
                if(_goFITSdk.isBLEConnect() ){
                    watchstatus.setText("Connectd!");

                }
                else {
                    watchstatus.setText("Disconnectd!");

                }
                handler.postDelayed(this,10000);
            }
        };

        handler.postDelayed(runnable,10000);

    }


    //implement  SDK  initialize
    public void initSDK() {
        //取得權限
        //檢查是否取得權限
        int permissionCheck = ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);

        //沒有權限時
        if (permissionCheck != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(Main2Activity.this,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                    1);
        }

        final String _tag = "SDK init";
        if (_goFITSdk == null) {
            // Read license if exist in local storage
            SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
            SharedPreferences.Editor pe = sp.edit();
            sdk_license = sp.getString("sdk_license", null);
            pe.apply();

            _goFITSdk = GoFITSdk.getInstance(this, sdk_certificate, sdk_license, new GoFITSdk.ReceivedLicenseCallback() {
                @Override
                public void onSuccess(String receivedLicense) {
                    Log.i(_tag, receivedLicense);
                    sdk_license = receivedLicense;

                    // Store license in local storage
                    SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(Main2Activity.this);
                    SharedPreferences.Editor pe = sp.edit();
                    pe.putString("sdk_license", sdk_license);
                    pe.commit();

                    Log.i(_tag, "SDK init OK : \n" + sdk_license);
                }

                @Override
                public void onFailure(int errorCode, String errorMsg) {
                    Log.e(_tag, "GoFITSdk.getInstance() : (callback) onFailure() : errorCode = " + errorCode + ", " + "errorMsg = " + errorMsg);
                    Log.i(_tag, "SDK init Error : \n" + errorMsg);
                }
            });
            _goFITSdk.reInitInstance();
            Toast.makeText(this, "SDK init!", Toast.LENGTH_SHORT).show();
        }
        else {
            _goFITSdk.reInitInstance();
            Toast.makeText(this, "SDK init!", Toast.LENGTH_SHORT).show();
        }
    }

    //scan device and paring and let device keep tracking heart rate (incldue 3 api : scan,paring,setting)
    public void scan(View view){
        //Scan Device
        final String tag = "Scan Function";
        Log.d("MAIN", "scan: start");
        _goFITSdk.doScanDevice(new GoFITSdk.DeviceScanCallback() {
            @Override
            public void onSuccess(ScanBluetoothDevice device) {
                Log.i(tag, "DeviceScanCallback.onSuccess() : device () = " + device.toString());
            }

            @Override
            public void onCompletion(ArrayList<ScanBluetoothDevice> arrayList) {
                for(int i = 0; i < arrayList.size(); i++){
                    ScanBluetoothDevice device = arrayList.get(i);
                    ScanResult = device;
                    Log.i(tag,"DeviceScanCallback.onCompletion() :"+ device.getDevice().getName() + ", "
                            + device.getDevice().getAddress() + ", "
                            + device.getRSSI());
                }

                //New paring
                _goFITSdk.doNewPairing(ScanResult, new GoFITSdk.NewPairingCallback() {
                    @Override
                    public void onSuccess(String s, final String s1) {
                        Log.i("MAIN", "onSuccess: 取得配對");

                        //Comfirm  paring  code
                        final Dialog d = new Dialog(Main2Activity.this);
                        d.setTitle("驗證碼確認");
                        d.setContentView(R.layout.dialog_scancode);
                        d.show();

                        Button paring_btn = d.findViewById(R.id.paring_btn);
                        paring_btn.setOnClickListener(new View.OnClickListener() {
                            @Override
                            public void onClick(final View view) {
                                EditText paring_code = d.findViewById(R.id.paring_code);
                                final String newcode = paring_code.getText().toString();
                                _goFITSdk.doConfirmPairingCode(newcode, s1, ScanResult.getProductID(), new GoFITSdk.GenericCallback() {
                                    @Override
                                    public void onSuccess() {
                                        d.cancel();
                                        Log.i(tag, "onSuccess: 配對成功"+ ScanResult.getDevice().getName() + ", "
                                                + ScanResult.getDevice().getAddress() + ", "
                                                + ScanResult.getRSSI());


                                        //Store watch information

                                        _goFITSdk.getDeviceMAC(new GoFITSdk.GetDeviceInfoCallback() {
                                            @Override
                                            public void onSuccess(String s) {
                                                mac = s;
                                                SharedPreferences setting_watch =getSharedPreferences("watch_info",MODE_PRIVATE);

                                                setting_watch.edit().putString("macAddress",mac)
                                                        .putString("productID",ScanResult.getProductID())
                                                        .putString("pairingCode",newcode)
                                                        .putString("pairingTime",s1)
                                                        .commit();

                                                Log.d("MAIN", mac+"_"+ScanResult.getProductID()+"_"+newcode+"_"+s1);
                                            }

                                            @Override
                                            public void onFailure(int i, String s) {

                                            }
                                        });


                                        //Sync data once when success pairing device
                                        GetHealthData();

                                        //set heart rate detect timing
                                        final String _tag = "Device Setting";
                                        SetCareSetting mCareSettings = _goFITSdk.getDefaultCareSettings();
                                        CareMeasureHR careMeasureHR = mCareSettings.getDefaultMeasureHR();
                                        careMeasureHR.setRepeatDays(convertRepeatDay(127));
                                        careMeasureHR.setEnableMeasureHR(true);
                                        careMeasureHR.setStartMin((short)(0000));
                                        careMeasureHR.setEndMin((short)2359);
                                        careMeasureHR.setInterval((short)1);
                                        _goFITSdk.doSetSettings(mCareSettings, new GoFITSdk.SettingsCallback() {
                                            @Override
                                            public void onCompletion() {
                                                Log.i(_tag, "onCompletion: Device setting complete ");
                                            }

                                            @Override
                                            public void onProgress(String s) {

                                            }

                                            @Override
                                            public void onFailure(int i, String s) {
                                                Log.i(_tag, "onFailure: " + s);
                                            }
                                        });


                                    }

                                    @Override
                                    public void onFailure(int i, String s) {

                                    }
                                });
                            }
                        });

                    }

                    @Override
                    public void onFailure(int i, String s) {
                        Log.i(tag, "onFailure: 配對失敗");
                    }
                });
            }

            @Override
            public void onFailure(int errorCode, String errorMsg) {
                Log.i(tag, "onFailure ");
                Log.i(tag,
                        "DeviceScanCallback.onFailure() : errorCode = " + errorCode
                                + ", errorMsg = " + errorMsg
                );
            }
        });

    }

    //cancel device connection
    public void cancel(View view){
        Log.d("MAIN", "cancel: ");
        _goFITSdk.doSendIncomingMessage(AppContract.emIncomingMessageType.Default,"結束","end");
        Log.d("MAIN", "cancel2: ");
        _goFITSdk.doDisconnectDevice();
    }

    //get health data and clear data (include 2 api)
    public void GetHealthData(){

        final Handler ToastHandler = new Handler();
        final String _tag = "MAIN";
        Log.d("MAIN", "GetHealthData: ");
        if(_goFITSdk.isBLEConnect()){
            Log.d(_tag, "GetHealthData: Connected");
        }
        else{
            Log.d(_tag, "GetHealthData: Disconnected");
        }

        _goFITSdk.doSyncFitnessData(new GoFITSdk.SyncCallback() {
            @Override
            public void onCompletion() {
                isSync = false;
                ToastHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(Main2Activity.this, "Sync complete!", Toast.LENGTH_SHORT).show();
                    }
                });

                //clear data
                _goFITSdk.doClearDeviceData(new GoFITSdk.GenericCallback() {
                    @Override
                    public void onSuccess() {
                        ToastHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(Main2Activity.this, "Success Clear Data", Toast.LENGTH_SHORT).show();
                            }
                        });
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
                for(int i = 0; i < stepRecords.size(); i++){
                    Log.i(_tag, "doSyncFitnessData() : onGetFitnessData() : step = " + stepRecords.get(i).getSteps() + "  timestamp : " + stepRecords.get(i).getTimestamp());
                    if(i == (stepRecords.size()-1)){
                        Log.d(_tag, "Current step: " + stepRecords.get(i).getSteps());
                        //step=Integer.toString(stepRecords.get(i).getSteps());
                        //step_value.setText(Integer.toString(stepRecords.get(i).getSteps()));
                    }
                }

                for (TableSleepRecord sleep : sleepRecords) {
                    Log.i(_tag, "doSyncFitnessData() : onGetFitnessData() : sleep = " + sleep.toJSONString());
                }

                for(int i = 0; i < hrRecords.size(); i++){
                    Log.i(_tag, "doSyncFitnessData() : onGetFitnessData() : hr = " + hrRecords.get(i).getPulse() + "  timestamp : " + hrRecords.get(i).getTimestamp());
                    if(i == (hrRecords.size()-1)){
                        Log.d(_tag, "Current HR: " + hrRecords.get(i).getPulse());
                        //hr=Integer.toString(hrRecords.get(i).getPulse());
                        //hr_value.setText(Integer.toString(hrRecords.get(i).getPulse()));
                    }
                }

                for(int i = 0; i < spo2Records.size(); i++){
                    Log.i(_tag, "doSyncFitnessData() : onGetFitnessData() : hr = " + spo2Records.get(i).getSpO2() + "  timestamp : " + spo2Records.get(i).getTimestamp());
                    if(i == (spo2Records.size()-1)){
                        Log.d(_tag, "Current o2: " + spo2Records.get(i).getSpO2());
                        //o2=Integer.toString(spo2Records.get(i).getSpO2());
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



    //for set device
    byte[] convertRepeatDay(int days) {
        byte[] repeatDays = {0, 0, 0, 0, 0, 0, 0};
        try {
            repeatDays[0] = (byte) (((days & 0x01) == 1) ? 1 : 0);
            repeatDays[1] = (byte) ((((days >> 1) & 0x01) == 1) ? 1 : 0);
            repeatDays[2] = (byte) ((((days >> 2) & 0x01) == 1) ? 1 : 0);
            repeatDays[3] = (byte) ((((days >> 3) & 0x01) == 1) ? 1 : 0);
            repeatDays[4] = (byte) ((((days >> 4) & 0x01) == 1) ? 1 : 0);
            repeatDays[5] = (byte) ((((days >> 5) & 0x01) == 1) ? 1 : 0);
            repeatDays[6] = (byte) ((((days >> 6) & 0x01) == 1) ? 1 : 0);
        } catch (Exception e) {
            for (int i = 0; i < repeatDays.length; i++) {
                repeatDays[i] = 0;
            }
        }

        return repeatDays;
    }

}
