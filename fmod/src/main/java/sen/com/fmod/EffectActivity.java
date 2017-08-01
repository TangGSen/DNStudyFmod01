package sen.com.fmod;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.view.View;

import org.fmod.FMOD;

import java.io.File;

/**
 * Created by Administrator on 2017/8/1.
 */

public class EffectActivity extends Activity {

    private String filePath;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        FMOD.init(this);
        filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + File.separator + "Download" + File.separator + "test.mp3";

    }

    public void typeNormal(View view) {
        new Thread() {
            @Override
            public void run() {
                EffectUtils.playNormal(filePath, EffectUtils.TYPE_NORMAL);
            }
        }.start();

    }

    public void typeLuoLi(View view) {
        new Thread() {
            @Override
            public void run() {
                EffectUtils.playNormal(filePath, EffectUtils.TYPE_LUOLI);
            }
        }.start();

    }

    public void typeDaShu(View view) {
        new Thread() {
            @Override
            public void run() {
                EffectUtils.playNormal(filePath, EffectUtils.TYPE_LUOLI);
            }
        }.start();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FMOD.close();
    }
}
