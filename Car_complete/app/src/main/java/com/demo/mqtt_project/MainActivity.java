package com.demo.mqtt_project;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;



public class MainActivity extends AppCompatActivity {
    private String host = "tcp://39.97.245.18:1883";
    private String userName = "android";
    private String passWord = "android";
    private String mqtt_id = "474141956"; //定义成自己的QQ号  切记！不然会掉线！！！
    private String mqtt_sub_topic = "474141956"; //为了保证你不受到别人的消息  哈哈
    private String mqtt_pub_topic = "474141956_ESP"; //为了保证你不受到别人的消息  哈哈  自己QQ好后面加 _PC
    private int led_flag =1;
    private ScheduledExecutorService scheduler;
    private Button btn_1;  //类似于单片机开发里面的   参数初始化
    private ImageView image_1;
    private ImageButton bup,bdown,bleftup,bleftdown,brightdown,brightup,bleft,bon,bright;
    private TextView text_test;
    private MqttClient client;
    private MqttConnectOptions options;
    private Handler handler;

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main); // 对应界面UI


        bup=findViewById(R.id.up);
        bdown=findViewById(R.id.down);
        bleft=findViewById(R.id.left);
        bright=findViewById(R.id.right);
        bleftdown=findViewById(R.id.leftdown);
        bleftup=findViewById(R.id.leftup);
        brightup=findViewById(R.id.rightup);
        brightdown=findViewById(R.id.rightdown);
        bon=findViewById(R.id.on);

        bup.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":1}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        bright.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":2}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        bleft.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":3}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        bdown.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":4}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        bleftup.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":5}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        bleftdown.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":6}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        brightdown.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":7}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        brightup.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        Log.e("xxx","按下");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":8}");
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.e("xxx","抬起");
                        publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
                        break;

                }
                return true;
            }
        });
        bon.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e("xxx","抬起");
                publishmessageplus(mqtt_pub_topic,"{\"set_led\":0}");
            }
        });




//**********************************************************//
        Mqtt_init();
        startReconnect();
        handler = new Handler() {
            @SuppressLint("SetTextI18n")
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what){
                    case 1: //开机校验更新回传
                        break;
                    case 2:  // 反馈回传

                        break;
                    case 3:  //MQTT 收到消息回传   UTF8Buffer msg=new UTF8Buffer(object.toString());
                        //处理message 传过来的 obj字段（里面包了数据）

                        String T_val = msg.obj.toString().substring(msg.obj.toString().indexOf("temperature\":")+13,msg.obj.toString().indexOf("}"));
                        String text_val = "温度："+T_val;
                        //在主进程 handler 里面更新UI  既保证了稳定性  又不影响网络传输
                //        text_test.setText(text_val);
                        //Toast.makeText(MainActivity.this,T_val ,Toast.LENGTH_SHORT).show();

                        break;
                    case 30:  //连接失败
                        Toast.makeText(MainActivity.this,"连接失败" ,Toast.LENGTH_SHORT).show();
                        break;
                    case 31:   //连接成功
                        Toast.makeText(MainActivity.this,"连接成功" ,Toast.LENGTH_SHORT).show();
                        try {
                            client.subscribe(mqtt_sub_topic,1);
                        } catch (MqttException e) {
                            e.printStackTrace();
                        }
                        break;
                    default:
                        break;
                }
            }
        };
    }

    private void Mqtt_init()
    {
        try {
            //host为主机名，test为clientid即连接MQTT的客户端ID，一般以客户端唯一标识符表示，MemoryPersistence设置clientid的保存形式，默认为以内存保存
            client = new MqttClient(host, mqtt_id,
                    new MemoryPersistence());
            //MQTT的连接设置
            options = new MqttConnectOptions();
            //设置是否清空session,这里如果设置为false表示服务器会保留客户端的连接记录，这里设置为true表示每次连接到服务器都以新的身份连接
            options.setCleanSession(false);
            //设置连接的用户名
            options.setUserName(userName);
            //设置连接的密码
            options.setPassword(passWord.toCharArray());
            // 设置超时时间 单位为秒
            options.setConnectionTimeout(10);
            // 设置会话心跳时间 单位为秒 服务器会每隔1.5*20秒的时间向客户端发送个消息判断客户端是否在线，但这个方法并没有重连的机制
            options.setKeepAliveInterval(20);
            //设置回调
            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    //连接丢失后，一般在这里面进行重连
                    System.out.println("connectionLost----------");
                    //startReconnect();
                }
                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    //publish后会执行到这里
                    System.out.println("deliveryComplete---------"
                            + token.isComplete());
                }
                @Override
                public void messageArrived(String topicName, MqttMessage message)
                        throws Exception {
                    //subscribe后得到的消息会执行到这里面
                    System.out.println("messageArrived----------");
                    Message msg = new Message();
                    //封装message包
                    msg.what = 3;   //收到消息标志位
                    msg.obj = topicName + "---" + message.toString();
                    //发送messge到handler
                    handler.sendMessage(msg);    // hander 回传
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    private void Mqtt_connect() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if(!(client.isConnected()) )  //如果还未连接
                    {
                        client.connect(options);
                        Message msg = new Message();
                        msg.what = 31;
                        // 没有用到obj字段
                        handler.sendMessage(msg);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Message msg = new Message();
                    msg.what = 30;
                    // 没有用到obj字段
                    handler.sendMessage(msg);
                }
            }
        }).start();
    }
    private void startReconnect() {
        scheduler = Executors.newSingleThreadScheduledExecutor();
        scheduler.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                if (!client.isConnected()) {
                    Mqtt_connect();
                }
            }
        }, 0 * 1000, 10 * 1000, TimeUnit.MILLISECONDS);
    }
    private void publishmessageplus(String topic,String message2)
    {
        if (client == null || !client.isConnected()) {
            return;
        }
        MqttMessage message = new MqttMessage();
        message.setPayload(message2.getBytes());
        try {
            client.publish(topic,message);
        } catch (MqttException e) {

            e.printStackTrace();
        }
    }





}
