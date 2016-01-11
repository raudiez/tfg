package es.uca.tfg_rau.deafalert;

import android.app.Activity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by tfg_rau on 2/01/16.
 */
public class AlertListAdapter extends BaseAdapter{
    private Activity activity;
    private ArrayList<String> alerts;
    private static LayoutInflater inflater = null;


    public AlertListAdapter(Activity activity, ArrayList<String> alerts){
        this.activity = activity;
        this.alerts = alerts;
        this.inflater = activity.getLayoutInflater();
    }

    @Override
    public int getCount() {
        return this.alerts.size();
    }

    @Override
    public Object getItem(int position) {
        return position;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View vi = convertView;
        ViewHolder holder;

        if(convertView==null){
            vi = inflater.inflate(R.layout.item,null);
            holder = new ViewHolder();
            holder.raspberryText = (TextView)vi.findViewById(R.id.textRaspberry);
            vi.setTag(holder);
        }else{
            holder = (ViewHolder)vi.getTag();
        }
        holder.raspberryText.setText(this.alerts.get(position));
        return vi;
    }


    public void setData(String newalert){
        this.alerts.add(newalert);
        this.notifyDataSetChanged();
    }


    public void remove(int position){
        this.alerts.remove(position);
        this.notifyDataSetChanged();
    }

    public class ViewHolder
    {
        TextView raspberryText;
    }
}
