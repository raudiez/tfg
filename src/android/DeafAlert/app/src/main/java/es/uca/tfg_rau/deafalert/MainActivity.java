package es.uca.tfg_rau.deafalert;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.TaskStackBuilder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.StrictMode;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.app.NotificationCompat;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    private InetAddress raspberryAddr;
    private ListView alertList;
    private ArrayList<String> alerts;
    private AlertListAdapter alertAdapter;
    private TextView textRaspberry;
    private int NotifCount;
    private boolean appIsInForegroundMode;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        textRaspberry = (TextView) findViewById(R.id.textRaspberry);
        alertList = (ListView) findViewById(R.id.alertList);
        alerts = new ArrayList<>();
        alertAdapter = new AlertListAdapter(MainActivity.this, alerts);
        alertList.setAdapter(alertAdapter);
        NotifCount = 0;
        appIsInForegroundMode = false;
        Timer timer = new Timer();
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                printRaspberryData();
            }
        }, 0, 1500);
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.settings_menu:
                Intent intent = new Intent(this, SettingsActivity.class);
                this.startActivity(intent);
                break;
            default:
                return super.onOptionsItemSelected(item);
        }
        return true;
    }

    @Override
    protected void onResume(){
        super.onResume();
        appIsInForegroundMode = true;
        alertList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, final int position, long id) {
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setMessage(getText(R.string.question))
                        .setCancelable(true)
                        .setNeutralButton(getText(R.string.cancel), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                dialog.cancel();
                            }

                        })
                        .setPositiveButton(getText(R.string.okey), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                alertAdapter.remove(position);
                            }
                        });
                AlertDialog alert = builder.create();
                alert.show();
            }
        });
        registerForContextMenu(alertList);
    }

    @Override
    protected void onPause(){
        super.onPause();
        appIsInForegroundMode = false;
    }

    private void printRaspberryData(){this.runOnUiThread(printAlerts);}

    private void notifyAlertToNotification(String cad){
        NotificationCompat.Builder mBuilder = (NotificationCompat.Builder) new NotificationCompat.Builder(this)
                .setSmallIcon(R.drawable.ic_notify_icon)
                .setContentTitle("DeafAlert")
                .setContentText(cad)
                .setLights(Color.WHITE, 500, 1000)
                .setOnlyAlertOnce(true)
                .setPriority(2)
                .setVibrate(new long[] { 1000, 500, 500, 500, 500 })
                .setAutoCancel(true);
        Intent resultIntent = new Intent(this, MainActivity.class);

        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addParentStack(MainActivity.class);
        stackBuilder.addNextIntent(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);
        mBuilder.setContentIntent(resultPendingIntent);
        NotificationManager mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.notify(NotifCount, mBuilder.build());
        NotifCount++;
    }

    private Runnable printAlerts = new Runnable() {
        @Override
        public void run() {
            ConnectivityManager connManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo wifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
            boolean stop = false;
            if (wifi != null && wifi.isConnected()) {
                SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(MainActivity.this);
                String raspberry = sharedPref.getString("ip_address", getText(R.string.pref_default_ip_address).toString());

                DatagramSocket socket;
                try {
                    raspberryAddr = InetAddress.getByName(raspberry);
                    socket = new DatagramSocket(8081);
                    socket.connect(raspberryAddr, 8080);
                    if(socket.isConnected()){
                        byte[] reqBuff = "GET /getAlert".getBytes();
                        DatagramPacket reqPacket = new DatagramPacket(reqBuff, reqBuff.length, raspberryAddr, 8080);
                        try {
                            socket.send(reqPacket);
                        } catch (IOException e) {
                            e.printStackTrace();
                            stop = true;
                            textRaspberry.setText(getText(R.string.mainactivity_send_packet_error));
                        }
                        if(!stop) {
                            //Hecha petición, ahora espera la respuesta:
                            boolean stop2 = false;
                            byte[] recvBuff = new byte[1024];
                            DatagramPacket recvPacket = new DatagramPacket(recvBuff, recvBuff.length);
                            try {
                                socket.receive(recvPacket);
                            } catch (IOException e) {
                                textRaspberry.setText(getText(R.string.mainactivity_no_data_error));
                                e.printStackTrace();
                                stop2 = true;
                            }
                            if (!stop2) {
                                String cad = new String(recvPacket.getData(), 0, recvPacket.getLength());
                                textRaspberry.setText(getText(R.string.mainactivity_conn_ok));
                                alertAdapter.setData(cad);
                                if(!appIsInForegroundMode) notifyAlertToNotification(cad);
                                //Log.d("Data from raspberry:", " > " + cad);
                            }
                            socket.close();
                        }//1st stop
                    }else{
                        textRaspberry.setText(getText(R.string.mainactivity_conn_error));
                        socket.close();
                    }
                    socket.close();
                } catch (Exception e) {
                    textRaspberry.setText(getText(R.string.mainactivity_server_conn_error));
                    e.printStackTrace();
                }

            } else {
                textRaspberry.setText(getText(R.string.mainactivity_wifi_error));
            }
        }

    };
}
