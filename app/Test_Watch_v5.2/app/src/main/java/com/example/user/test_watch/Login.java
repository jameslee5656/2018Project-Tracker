package com.example.user.test_watch;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.afollestad.materialdialogs.DialogAction;
import com.afollestad.materialdialogs.MaterialDialog;
import com.example.user.test_watch.Retrofit.IMyService;
import com.example.user.test_watch.Retrofit.RetrofitClient;
import com.github.javiersantos.materialstyleddialogs.MaterialStyledDialog;
import com.rengwuxian.materialedittext.MaterialEditText;

import io.reactivex.Scheduler;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;
import retrofit2.Retrofit;
import  static com.example.user.test_watch.App.Name;

public class Login extends AppCompatActivity {

    TextView txt_creare_account;
    MaterialEditText edt_login_name,edt_login_password;
    Button login_btn;

    CompositeDisposable compositeDisposable = new CompositeDisposable();
    IMyService iMyService;

    @Override
    protected void onStop() {
        compositeDisposable.clear();
        super.onStop();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        //Init Service
        Retrofit retrofitClient = RetrofitClient.getInstance();
        iMyService = retrofitClient.create(IMyService.class);


        //Init View
        edt_login_name = (MaterialEditText)findViewById(R.id.edt_name);
        edt_login_password = (MaterialEditText)findViewById(R.id.edt_password);
        SharedPreferences setting =getSharedPreferences("Login",MODE_PRIVATE);
        edt_login_name.setText(setting.getString("Name",""));
        edt_login_password.setText(setting.getString("Password",""));

        //check Network
        checkNetWork();

        login_btn = (Button) findViewById(R.id.login_btn);
        login_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!checkNetWork()){
                    return;
                }
                loginUser(edt_login_name.getText().toString(),
                        edt_login_password.getText().toString());
            }
        });

        txt_creare_account = (TextView)findViewById(R.id.txt_create_account);
        txt_creare_account.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final View register_layout = LayoutInflater.from(Login.this)
                        .inflate(R.layout.register_layout,null);
                new MaterialStyledDialog.Builder(Login.this)
                        .setIcon(R.drawable.ic_user)
                        .setTitle("註冊")
                        .setDescription("請填入下方資料")
                        .setCustomView(register_layout)
                        .setNegativeText("取消")
                        .onNegative(new MaterialDialog.SingleButtonCallback() {
                            @Override
                            public void onClick(@NonNull MaterialDialog dialog, @NonNull DialogAction which) {
                                dialog.dismiss();
                            }
                        })
                        .setPositiveText("確認")
                        .onPositive(new MaterialDialog.SingleButtonCallback() {
                            @Override
                            public void onClick(@NonNull MaterialDialog dialog, @NonNull DialogAction which) {

                                if(!checkNetWork()){
                                    return;
                                }

                                MaterialEditText edt_register_name = (MaterialEditText)register_layout.findViewById(R.id.edt_name);
                                MaterialEditText edt_register_password = (MaterialEditText)register_layout.findViewById(R.id.edt_password);


                                if(TextUtils.isEmpty(edt_register_name.getText().toString())){
                                    Toast.makeText(Login.this, "名稱 不能空白", Toast.LENGTH_SHORT).show();
                                    return;
                                }
                                if(TextUtils.isEmpty(edt_register_name.getText().toString())){
                                    Toast.makeText(Login.this, "名字 不能空白", Toast.LENGTH_SHORT).show();
                                    return;
                                }

                                if(TextUtils.isEmpty(edt_register_password.getText().toString())){
                                    Toast.makeText(Login.this, "密碼 不能空白", Toast.LENGTH_SHORT).show();
                                    return;
                                }

                                registerUser(edt_register_name.getText().toString(),
                                        edt_register_password.getText().toString());
                            }
                        }).show();
            }
        });


    }

    private void registerUser(String name, String password) {
        compositeDisposable.add(iMyService.registerUser(name,password)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Consumer<String>() {
                    @Override
                    public void accept(String response) throws Exception {
                        Toast.makeText(Login.this, ""+response, Toast.LENGTH_SHORT).show();
                    }
                }));
    }

    private void loginUser(final String name, final String password) {
        if(TextUtils.isEmpty(name)){
            Toast.makeText(this, "名稱 不能空白", Toast.LENGTH_SHORT).show();
            return;
        }
        if(TextUtils.isEmpty(password)){
            Toast.makeText(this, "密碼 不能空白", Toast.LENGTH_SHORT).show();
            return;
        }
        Name = name;
        Log.d("LOGIN", Name);
        compositeDisposable.add(iMyService.loginrUser(name,password)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Consumer<String>() {
                    @Override
                    public void accept(String response) throws Exception {
                        Toast.makeText(Login.this, "" + response, Toast.LENGTH_SHORT).show();
                        if(response.equals("\"Login success\"")){
                            SharedPreferences setting =getSharedPreferences("Login",MODE_PRIVATE);
                            setting.edit().putString("Name",name)
                                        .putString("Password",password)
                                        .commit();
                            Intent intent = new Intent();
                            intent.setClass(Login.this, Main2Activity.class);
                            startActivity(intent);
                        }
                    }
                }));
    }

    private boolean checkNetWork() {
        ConnectivityManager mConnectivityManager =
                (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mNetworkInfo = mConnectivityManager.getActiveNetworkInfo();

        if (mNetworkInfo ==null){
            new AlertDialog.Builder(this).setMessage("沒有網路")
                    .setPositiveButton("前往設定網路", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            Intent callNetSettingIntent = new Intent(
                                    Settings.ACTION_SETTINGS);
                            Toast.makeText(Login.this, "請前往開啟網路", Toast.LENGTH_LONG).show();
                            startActivity(callNetSettingIntent);
                        }
                    }).show();
            return false;
        }else{
            return true;
        }

    }
}

