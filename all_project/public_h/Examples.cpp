#include"public.h"

int main()
{
	//初始化日志系统
	CIULOG::init_ciulog(true,false);
	//写入日志
	CIULOG::LOG_INFO("heelo");

	return 0;
}
