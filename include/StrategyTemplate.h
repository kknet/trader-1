#ifndef STRATEGYTEMPLATE_H
#define STRATEGYTEMPLATE_H
#include<map>
#include<set>
#include<memory>
#include<atomic>
#include<vector>
#include"structs.h"
#include"CTAAPI.h"
#include"json11.h"
#include"AlgorithmOrder.h"
#include"MongoCxx.h"
#define BAR_MODE "barmode"
#define TICK_MODE "tickmode"

class algorithmOrder;
class MongoCxx;
class CTPAPI;

class StrategyData
{
public:
	void insertparam(std::string key, std::string value);					//�������
	void insertvar(std::string key, std::string value);						//�������
	std::string getparam(std::string);										//��ȡ����
	std::string getvar(std::string);										//��ȡ����
	std::map<std::string, std::string>getallparam();						//��ȡ����
	std::map<std::string, std::string>getallvar();							//��ȡ����
private:
	std::mutex m_mtx;					//������
	std::map<std::string, std::string>m_parammap;//�����б�
	std::map<std::string, std::string>m_varmap;//�����б�
};

class StrategyTemplate
{
public:
	StrategyTemplate(CTAAPI *ctamanager);
	~StrategyTemplate();
	/******************************���Բ����ͱ���*********************************************/
	//��������
	std::string gatewayname;				//CTP
	std::string tickDbName;					//tick���ݿ���
	std::string BarDbName;					//Bar���ݿ���
	//��������
	std::string trademode;					//����ģʽ
	bool inited;							//��ʼ������
	bool trading;							//���׿���
	int initDays;							//������ʷ���ݵ�����

	//�㷨���ײ��ֱ���						Ĭ��ֵ�����Բ��޸��ڲ���ʵ���е���setlimit��������������
	double unitLimit;						//�㷨�µ�
	Mode TradingMode;				//�ز⻹��ʵ��

	/******************************CTAMANAGER���Ʋ���***********************************************/
	//��ʼ��
	virtual void onInit();
	//��ʼ 
	virtual void onStart();
	//ֹͣ
	virtual void onStop();
	//�����е���ֹͣ����ʱʹ��
	void cancelallorder();
	//��������ֵ
	void checkparam(const char* paramname, const char* paramvalue);
	//��pos��ֵ
	void checkSymbol(const char* symbolname);
	//��ȡ������ֵ
	std::string getparam(std::string paramname);
	//���ز������ݵ�����
	void putGUI();
	//���²���������
	virtual void putEvent();
	//tradeevent���³ֲ�
	void changeposmap(std::string symbol, double pos);
	/*******************************ʵ�ֲ�����Ҫ����**************************************************/
	//TICK
	virtual void onTick(TickData Tick);
	//BAR
	virtual void onBar(BarData Bar);
	//�����ص�
	virtual void onOrder(std::shared_ptr<Event_Order>e);
	//�ɽ��ص�
	virtual void onTrade(std::shared_ptr<Event_Trade>e);

	//��������

	//����
	std::vector<std::string> buy(std::string symbol, double price, double volume);
	//ƽ��
	std::vector<std::string> sell(std::string symbol, double price, double volume);
	//����
	std::vector<std::string> sellshort(std::string symbol, double price, double volume);
	//ƽ��
	std::vector<std::string> buycover(std::string symbol, double price, double volume);

	//��������
	void cancelOrder(std::string orderID, std::string gatewayname);

	//�㷨����
	double getpos(std::string symbol);							//���㷨����ģ���ȡ�ֲ��ⲿ�ӿ�
	std::map<std::string, double>getposmap();					//��ȡȫ���ֲ֣����㷨����������

	//�ṩ��backtestengine���ⲿ�ӿ�
	std::map<std::string, std::string> GetVarPlotMap();
	std::map<std::string, std::string> GetIndicatorMap();


	//�㷨����
	algorithmOrder *m_algorithm;
protected:
	//��ȡ��ʷ����
	std::vector<TickData>loadTick(std::string symbol, int days);
	std::vector<BarData>loadBar(std::string symbol, int days);

	//����mongocxxģ�屣��Ͷ�ȡmongodb
	virtual void savepostomongo();
	virtual void loadposfrommongo();
	MongoCxx *m_MongoCxx;

	//CTA������
	CTAAPI *m_ctamanager;

	//Bar����                                   tick�����Բ��� �� �����bar�ľ�Ҫ��
	std::mutex m_hourminutemtx;					//�����ڼ̳еĲ���ֱ������Щ������ʡ�������� 
	int m_minute;
	int m_hour;
	BarData m_bar;

	//���������б�
	StrategyData *m_strategydata;

	/**********************************_(:�١���)_�ز�ר������***********************************************/
	std::map<std::string, std::string>m_VarPlot;	std::map<std::string, std::string>m_indicatorPlot;			std::mutex m_VarPlotmtx;
private:
	//�ܱ�����ƽ��������
	std::vector<std::string> sendOrder(std::string symbol, std::string orderType, double price, double volume);

	/*******************************************************/
	std::map<std::string, double>m_pos_map;				//�ֲ�
	std::mutex m_Pos_mapmtx;

	//OrderList
	std::mutex m_orderlistmtx;							//������
	std::vector<std::string>m_orderlist;				//��ͨ�����б�
};

class BarGreater
{
public:
	bool operator ()(const BarData& bar1, const BarData& bar2)
	{
		return bar1.unixdatetime < bar2.unixdatetime;
	}
};

class TickGreater
{
public:
	bool operator ()(const TickData& tick1, const TickData& tick2)
	{
		return tick1.unixdatetime < tick2.unixdatetime;
	}
};

#endif

/*
.h

std::map<std::string,int> m_barMinute;
std::map<std::string, int> m_barHour;
std::map<std::string, BarData> m_barmap;









.cpp

if (m_barMinute.find(Tick.symbol) == m_barMinute.end())
{
m_barMinute.insert(std::pair<std::string, int>(Tick.symbol, 99));
}
if (m_barHour.find(Tick.symbol) == m_barHour.end())
{
m_barHour.insert(std::pair<std::string, int>(Tick.symbol, 99));
}
putEvent();

int tickMinute = Tick.getminute();
int tickHour = Tick.gethour();

m_hourminutemtx.lock();

if ((tickMinute != m_barMinute[Tick.symbol]) || (tickHour != m_barHour[Tick.symbol]))
{
if (m_barmap.find(Tick.symbol) != m_barmap.end())
{//�ж������Լ�Ƿ�Ҫ�����bar
onBar(m_barmap[Tick.symbol]);
}
BarData bar;

bar.symbol = Tick.symbol;
bar.exchange = Tick.exchange;
bar.open = Tick.lastprice;
bar.high = Tick.lastprice;
bar.low = Tick.lastprice;
bar.close = Tick.lastprice;

bar.openPrice = Tick.openPrice;//���տ�
bar.highPrice = Tick.highPrice;//���ո�
bar.lowPrice = Tick.lowPrice;//���յ�
bar.preClosePrice = Tick.preClosePrice;//����

bar.upperLimit = Tick.upperLimit;//��ͣ
bar.lowerLimit = Tick.lowerLimit;//��ͣ

bar.volume = Tick.volume;
bar.openInterest = Tick.openInterest;

bar.date = Tick.date;
bar.time = Tick.time;
bar.unixdatetime = Tick.unixdatetime;
m_barmap[Tick.symbol] = bar;
m_barMinute[Tick.symbol] = tickMinute;
m_barHour[Tick.symbol] = tickHour;

}
else
{
m_barmap[Tick.symbol].high = std::max(m_barmap[Tick.symbol].high, Tick.lastprice);
m_barmap[Tick.symbol].low = std::min(m_barmap[Tick.symbol].low, Tick.lastprice);
m_barmap[Tick.symbol].close = Tick.lastprice;
m_barmap[Tick.symbol].highPrice = Tick.highPrice;
m_barmap[Tick.symbol].lowPrice = Tick.lowPrice;
m_barmap[Tick.symbol].volume = Tick.volume;
}
m_hourminutemtx.unlock();




































*/