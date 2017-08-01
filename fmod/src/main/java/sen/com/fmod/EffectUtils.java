package sen.com.fmod;

/**
 * Created by Administrator on 2017/8/1.
 */

public class EffectUtils {
    public static final int TYPE_NORMAL = 0 ;
    public static final int TYPE_LUOLI = 1 ;
    public static final int TYPE_DASHU = 2 ;

    public static native void playNormal(String path,int type);

    static {
        System.loadLibrary("fmod");
        System.loadLibrary("fmodL");
        System.loadLibrary("effect");
    }
}
