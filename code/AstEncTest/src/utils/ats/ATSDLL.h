#ifndef _ATSDLL_H_
#define _ATSDLL_H_
#pragma once


#ifdef ATS_DLL
  #define ATS_CLASS __declspec(dllexport)
#else
  #define ATS_CLASS __declspec(dllimport)
#endif

#define ATS_API EXTERN_C ATS_CLASS

#define ATS_OK  0
#define ATS_ERROR -1

//位移
#define OFS(n) (1 << n)

//上下行
enum EN_WAY_TYPE
{
    WAY_NO      = 0,
    WAY_UP      = 1,    // 上行
    WAY_DN      = 2,    // 下行
    WAY_BOTH    = 3,
};

//左右手
enum EN_HAND_TYPE
{
    HAND_LEFT   = 0,    //左手
    HAND_RIGHT  = 1     //右手
};


//线路
enum EN_LINE_TYPE
{
    LINE_UPPER  = 1, //高线路(站场图靠顶部)
    LINE_LOWER  = 2  //低线路(站场图靠底部)
};

//列车
enum EN_SIDE_TYPE
{
    SIDE_UPPER  = 1, //列车高侧(站场图靠顶部)
    SIDE_LOWER  = 2  //列车底侧(站场图靠底部)
};

//站台风格
enum EN_DOCK_STYLE
{
    ISLE_DOCK  = 0,    // 岛式站台
    SIDE_DOCK  = 1     // 侧式站台
};

// 停车轨类型
enum EN_PARK_TYPE
{
    PARK_DOCK,         //站台
    PARK_TEMP,         //存车线
    PARK_LINK         //转换轨
};

//信号显示
enum EN_ASPECT_TYPE
{
    ASPECT_ERROR   = 0,// 故障
    ASPECT_GREEN   = 0x1,// 绿灯
    ASPECT_YELLOW  = 0x2,// 黄灯
    ASPECT_BOTH    = (ASPECT_GREEN|ASPECT_YELLOW)
};

//系统等级
enum EN_SYSTEM_TYPE
{
    SYSTEM_NUL = 0,        // 未定义
    SYSTEM_CTC = 1,        // 移动闭塞
    SYSTEM_ITC = 2,         // 点式后备
    SYSTEM_IXL = 3,        // 联锁模式
};

//服务类型
enum EN_SERVICE_TYPE
{
    SERVICE_GCC = -1,       //工程车
    SERVICE_RGC = 0,        //人工车
    SERVICE_JHC,            //(1~499) 正常计划运营车
    SERVICE_TMCN,           //(500~799) 正常计划运营车
    SERVICE_TMCR,           //(800~850) 反向普通头码车
    SERVICE_TMCNS,          //(900~950) 正向专列载客头码车
    SERVICE_TMCRS,          //(851~899) 正向专列载客头码车
    SERVICE_TMCZ,           //(951~999) 正向不载客直达车
};

// 正向不载客直达车
enum EN_WORK_TYPE
{
    WORK_待命,
    WORK_唤醒,
    WORK_下线场段运行,
    WORK_洗车,
    WORK_巡道,
    WORK_正线服务,
    WORK_清扫,
    WORK_休眠,
};

//驾驶模式
enum EN_TRAIN_TYPE
{
    TRAIN_NUL,
    TRAIN_IXL_FST,
    TRAIN_EUM    = TRAIN_IXL_FST,  //EUM
    TRAIN_RM,                       //RM
    TRAIN_IXL_END,
    TRAIN_ITC_FST,
    TRAIN_CMI    = TRAIN_ITC_FST,  //点式手动
    TRAIN_AMI,                      //点式自动
    TRAIN_CMB,                      //点式手动-增强型
    TRAIN_AMB,                      //点式自动-增强型
    TRAIN_ITC_END,
    TRAIN_CTC_FST,
    TRAIN_CM     = TRAIN_CTC_FST,
    TRAIN_AM,                      //CTC, 自动驾驶
    TRAIN_ATB,                     //自动折返
    TRAIN_FTC_FST,
    TRAIN_FRM    = TRAIN_FTC_FST,  //FRM-远程FRM
    TRAIN_CAM,                      //CAM-蠕动驾驶, 自动防护人工驾驶
    TRAIN_FAM,                      //FAM-全自动驾驶模式
    TRAIN_FTC_END,
    TRAIN_CTC_END = TRAIN_FTC_END,
};

//车型
enum EN_MODEL_TYPE
{
    MODEL_B = 0,     //B型车
    MODEL_A,        //A型车
    MODEL_P,         //P型车(工程车)
};

//折返类型
enum EN_TURNBACK_TYPE
{
	TURNBACK_NUL = 0,    //未定义
	TURNBACK_STRA,       //直线折返
	TURNBACK_BEND,       //侧线折返
	TURNBACK_BENDP,      //侧线优先
	TURNBACK_STRAP,      //直线优先
};

//绑定
enum EN_BAND_TYPE
{
    BAND_NUL = 0,
    BAND_ATS = OFS(0),
    BAND_SHA = OFS(1),
    BAND_U3D = OFS(2),
    BAND_SIM = OFS(3),
};

//进路办理方式
enum EN_ARRANGE_TYPE
{
	ARRANGE_CMD = 0,      //人工模式
	ARRANGE_ARS,          //自排模式
	ARRANGE_FLT,          //车队模式/连续排列进路?
};

//路径类型
enum EN_XTD_TYPE
{
	XTD_TG, //直轨
	XTD_FG, //进岔
	XTD_NG, //定模
	XTD_RG, //反岔
};

//折返类型
enum EN_CYCLE_TYPE
{
	CYCLE_EJ=1,        //Cycle中的牵入基本进路
	CYCLE_EZ=2,        //Cycle中的牵入折返进路
	CYCLE_OJ=3,        //Cycle中的牵出基本进路
	CYCLE_OZ=4,        //Cycle中的牵出折返进路
	CYCLE_AJ=5,        //联锁自动触发基本进路
	CYCLE_AZ=6,        //联锁自动触发折返进路
};

//对标
enum EN_SSP_TYPE
{
	SSP_OK, // 对标正确
	SSP_NO, // 对标错误
};

// ATS类型
enum EN_ATSTYPE
{
	ATSTYPE_NO,
	ATSTYPE_CC,    //中心
	ATSTYPE_LC,    //现地工作站
	ATSTYPE_MC,    //监视工作站
	ATSTYPE_SC,    //大屏
	ATSTYPE_TC,    //教员
	ATSTYPE_XC,    //其他
	ATSTYPE_CI,    //联锁
};


//
enum EN_CONNEX_TYPE
{
    CONNEX_NO,
    CONNEX_JL,
    CONNEX_D,
    CONNEX_S,
    CONNEX_E,
    CONNEX_DD,    //带动
};

enum EN_MA_TYPE
{
	MA_ROUTE=0, //区段允许,并且是进路末端
	MA_BLOCK,   //区段禁止
	MA_TRAIN,   //区段允许,区段列车追踪
	MA_PARK,    //区段允许,区段停车对标
	MA_NPARK,   //区段允许,区段停车欠标
};


//更新
enum EN_UPDATE_TYPE
{
    UPDATE_ALL = 0,
    UPDATE_ONE = 1
};

/////////////////////////////////////////////////////////////
////***这个区域需要配置


#define RT          3.6f      //步进率
#define ATP_TRACK_END  3.0f   //列车头部到区段末端的距离
#define ATP_TRAIN_END  50.0f  //列车安全防护距离
// lyc:雄安列车长度
//#define JHC_TRAIN_LEN  119.06f  //车身长度
#define JHC_TRAIN_LEN  187.2f   //车身长度-雄安
#define GCC_TRAIN_LEN  17.292f  //工程车长度


#define MAX_LINE_SPEED 1000    //线路允许最大速度
#define MAX_DOCK_SPEED  40     //站台区域速度


#define ARRANGE_DEFAULT_TICK 1500  //排列直通进路时间
#define ARRANGE_TURNING_TICK 4200   //排列转岔进路时间

// lyc:雄安方向相反
//#define DIR_RIGHT    WAY_DN    //正线向右的行别
#define DIR_RIGHT    WAY_UP     //正线向右的行别-雄安


#define DEF_PARK     HAND_LEFT  //列车默认停靠左手
#define DOCK_UPPER   WAY_UP     //上部站台(站场图靠顶部)

// lyc:记录
// 车次号 = 服务号
// 列车识别号 = 目的号 + 服务号 + 序列号(跑了几次), 11281502903 = 112815 + 029 + 03
// 车次号, 是在找一个空闲列车, 不停车组号, 将列车名字当做成车组号
// 而列车对象是在解析联锁表时, 提取创建好的列车名
// 所以下发计划时, 列车的车组号是随机的, 和时刻表没有关系
////***这个区域需要配置
/////////////////////////////////////////////////////////////


// 判断各种驾驶模式
#define IS_FTC(m)  ((m)>=TRAIN_FTC_FST&&(m)<TRAIN_FTC_END)
#define IS_CTC(m)  ((m)>=TRAIN_CTC_FST&&(m)<TRAIN_CTC_END)
#define IS_ITC(m)  ((m)>=TRAIN_ITC_FST&&(m)<TRAIN_ITC_END)
#define IS_IXL(m)  ((m)>=TRAIN_IXL_FST&&(m)<TRAIN_IXL_END)
#define IS_ATP(m)  ((m)!=TRAIN_CMI)

#define IS_FTC_UPPER(m) ((m)>=TRAIN_FTC_FST&&(m)<TRAIN_FTC_END)
#define IS_CTC_UPPER(m) ((m)>=TRAIN_CTC_FST&&(m)<TRAIN_CTC_END)
#define IS_BTC_UPPER(m) ((m)!=TRAIN_CMB  &&(m)<TRAIN_CTC_END)
#define IS_ITC_UPPER(m) ((m)>=TRAIN_ITC_FST&&(m)<TRAIN_CTC_END)


// 判断高低线路?
#define DOCK2LINE(way)    ((way)==DOCK_UPPER?LINE_UPPER:LINE_LOWER)   //向左

#define DIR_LEFT        (WAY_UP+WAY_DN-DIR_RIGHT)  //向左
#define DOCK_LOWER      (WAY_UP+WAY_DN-DOCK_UPPER) //(站场图靠底部)

//方向转行别
#define DIR2WAY(dir)    (dir==DIR_RIGHT?DIR_RIGHT:DIR_LEFT)

//行别转方向
#define WAY2DIR(way)    DIR2WAY(way)

//

#define IS_WAY_UP(way)   ((way)==WAY_UP)
#define IS_WAY_DN(way)   ((way)==WAY_DN)

//行别向右?
#define IS_DIR_RIGHT(way)  (DIR_RIGHT==way)
#define IS_DIR_LEFT(way)   (DIR_LEFT==way)

//行别向右?
#define IS_HAND_LEFT(hand)  (HAND_LEFT==(hand))
#define IS_HAND_RIGHT(hand)  (HAND_RIGHT==(hand))

//线路高低
#define IS_LINE_UPPER(line) (LINE_UPPER==(line))
#define IS_LINE_LOWER(line) (LINE_LOWER==(line))

//计算左右
#define LINE2HAND(line,way,dock) \
	(IS_LINE_UPPER(line)?\
	(IS_DIR_RIGHT(way)&&(ISLE_DOCK==dock)||IS_DIR_LEFT(way)&&(SIDE_DOCK==dock)?HAND_RIGHT:HAND_LEFT):\
	(IS_DIR_RIGHT(way)&&(SIDE_DOCK==dock)||IS_DIR_LEFT(way)&&(ISLE_DOCK==dock)?HAND_RIGHT:HAND_LEFT))

//计算高低
#define LINE2SIDE(line,dock) \
    (IS_LINE_UPPER(line)?\
    (ISLE_DOCK==(dock)?SIDE_LOWER:SIDE_UPPER):\
    (SIDE_DOCK==(dock)?SIDE_LOWER:SIDE_UPPER))



// 主要用于: 拿到一个对象指针, 就能取到它的类型 ID, 判断是什么控件
#define TOX(x) #x

template<class T>
struct CLASS_KEY{};
#define SET_CLASS_KEY(class_name,class_key) \
    template<>struct CLASS_KEY<class_name>{enum {KEY = class_key}; static LPCTSTR na(){return TOX(class_name);} };

#define SET_CLASS_KEY2(class_name,class_type) \
    class class_name; \
    template<>struct CLASS_KEY<class_name>{enum {KEY = class_type}; static LPCTSTR na(){return TOX(class_name);} };


// 位的各种管理
#define SET_BIT(state,bit,set) \
    ((int&)state)=(set?(state|bit):(state&~(bit)))

#define HAS_BIT(state,bit) ((state&(bit))>0)
#define ALL_BIT(state,bit) ((state&(bit))==(bit))

#define MODIFY_VALUE(val,add,del) (val = (((val) << del) | (add)))
#define MODIFY_STATE(val,set,flag) MODIFY_VALUE(val, (set)?(flag):0, (!(set))?(flag):0)


// 结构体初始化清零
#define ZERO_STRUCT(struct_name) \
    struct_name(){memset(this,0,sizeof(*this));}

// 时刻表的结构体
struct DB_TTL
{
    DB_TTL()
    {
        ZeroMemory(this,sizeof(*this));
    }
	int              No;
	BYTE             DBVersion;
	BYTE             ID;          //服务号 = 车次号
	BYTE             Consist;     //联合
	BYTE             ID2;         //序列号
	BYTE             TripID;      //序列号
	BYTE             DIDValue;   //交路号
	BYTE             Type;        //服务类型: 乘客-PE-PP
	BYTE             Dir;         //方向
	int              ID3;        //索引-单个行程
	TCHAR            TrackName[32];//轨道
	int              Arrival;      //到站时间
	int              Departure;    //离站时间
	UINT             Color;        //颜色
	BYTE             PenStyle;     //画笔
	BYTE             Dir2;         //当Dir2=0,表示终点站
	BYTE             Tag;          //目标
	BYTE             Type2;        //类型2
	int              Sid;          //车站ID
	int              Pvid;         //车组号
	int              Dcid;         //目的地
	int              Tid;          //服务号
	int              Trip;         //序列号
	int              Per;          //百分比
	int              Way;          //方向
	int              Tag3;         //保留
};

#define DEFINE_DB_TTL


#endif

