package com.example.user.test_watch.Retrofit;

import io.reactivex.Observable;
import retrofit2.http.Field;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.POST;

public interface IMyService {
    @POST("register")
    @FormUrlEncoded
    Observable<String> registerUser(@Field("name") String name,
                                    @Field("password") String password);

    @POST("login")
    @FormUrlEncoded
    Observable<String> loginrUser(@Field("name") String name,
                                  @Field("password") String password);

    @POST("get")
    @FormUrlEncoded
    Observable<String> getData(@Field("name") String name);

    @POST("getDayData")
    @FormUrlEncoded
    Observable<String> getDayData(@Field("name") String name,
                                  @Field("year") int year,
                                  @Field("month") int month,
                                  @Field("day") int day);

    @POST("getHourData")
    @FormUrlEncoded
    Observable<String> getHourData(@Field("name") String name,
                                   @Field("year") int year,
                                   @Field("month") int month,
                                   @Field("day") int day,
                                   @Field("hour") int hour);

    @POST("getRank")
    @FormUrlEncoded
    Observable<String> getRank(@Field("name") String name,
                                   @Field("year") int year,
                                   @Field("month") int month,
                                   @Field("day") int day,
                                   @Field("hour") int hour);
}
