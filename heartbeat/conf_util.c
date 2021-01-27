#include "conf_util.h"

static void trim(char *strIn, char *strOut){
#ifdef DEBUG
	printf("%s: %s\n",__func__,strIn);
#endif
    char *start, *end, *temp;//定义去除空格后字符串的头尾指针和遍历指针
    temp = strIn;

    while (*temp == ' '){
        ++temp;
    }

    start = temp; //求得头指针
    temp = strIn + strlen(strIn) - 1; //得到原字符串最后一个字符的指针(不是'\0')

    while (*temp == ' ' || *temp == '\n'){
        --temp;
    }

    end = temp; //求得尾指针
    for(strIn = start; strIn <= end; ){
        *strOut++ = *strIn++;
    }
    *strOut = '\0';
}

void set_conf(int i, char *val, struct hb_conf *conf)
{
	switch(i)
	{
		case P_ADDR:
			conf->primary_addr = (char*)malloc(strlen(val)+1);
			strncpy(conf->primary_addr, val, strlen(val));
			break;
		case B_ADDR:
			conf->backup_addr= (char*)malloc(strlen(val)+1);
			strncpy(conf->backup_addr, val, strlen(val));
			break;
		case HB_PORT:
			conf->hb_port = atoi(val);
			break;
		case HB_TIMEOUT:
			conf->hb_timeout = atoi(val);
			break;
		case HB_TRY_TIMES:
			conf->try_times = atoi(val);
			break;
		case HB_WELCOME:
#ifdef DEBUG
			printf("do welcome %s\n", val);
#endif
			conf->welcome= (char*)malloc(strlen(val)+1);
			strncpy(conf->welcome,val, strlen(val));
			break;
		default:
			break;
	}
}

static void getValue(char * keyAndValue, const char **key, int cnt, struct hb_conf *conf){
    char *p = keyAndValue;
	char value[50];
	int i;

    p = strstr(p, "="); //找等号的位置
    if(p == NULL){
#ifdef DEBUG
        printf("没有找到字段分割符号\n");
#endif
        return;
    }
    p+= strlen("=");//指针后移到等号后面
    trim(p, value);//删除字符串前后的空格

	for(i=0; i<cnt; i++)
	{
		if(strstr(keyAndValue, key[i]) !=NULL)
		{
			set_conf(i, value, conf);
			break;
		}
	}
}
//int writeCFG(const char *filename/*in*/, const char *key/*in*/, const char *value/*in*/){
void write_conf(const char *filename, const struct hb_conf *conf)
{
		/*
    FILE *pf = NULL;
    char ftemp[flen] = {0}, fline[1024] = {0}, *fp;    //文件缓存数组
    long fsize = 0;
    int reg = 0;
    int exit = 0;
    int i = 0;　　int flen = 8*1024;

    pf = fopen(filename, "r+");
    if(pf == NULL){
        pf = fopen(filename, "w+");
    }
    //获得文件大小
    fseek(pf, 0, SEEK_END); // 将文件指针pf指向末尾
    fsize = ftell(pf);//获取文件开头到pf的长度
    if(fsize > flen){
        printf("文件不能超过8k\n");
        reg = -1;
        goto end;
    }
    fseek(pf, 0, SEEK_SET); //将文件指针指向开头

    //一行一行的读，如果存在key则修改value存到缓存数组中
    while(!feof(pf)){//未到文件结尾
        fgets(fline, 1024, pf);
        if(strstr(fline, key) != NULL && exit == 1)
            strcpy(fline, "");
        if(strstr(fline, key) != NULL && exit == 0){ //判断key是否存在
            exit = 1;
            sprintf(fline,"%s = %s\n", key, value);
        }
        
        printf("fline = %s\n", fline);
        strcat(ftemp, fline);
        
    }
    if(exit != 1){//如果不存在则把key value写入到最后一行
        sprintf(fline,"%s = %s\n", key, value);
        strcat(ftemp, fline);
    }
    if(pf != NULL){
        fclose(pf);
        pf = fopen(filename, "w+");
        fp = (char *)malloc(sizeof(char) * strlen(ftemp) + 1);
        strcpy(fp, ftemp);
        fp[strlen(fp) - 1] = EOF;
        fputs(fp, pf);
        if(fp != NULL){
            free(fp);
            fp = NULL;
        }
        fclose(pf);
    }
    end :
        if(pf != NULL)
            fclose(pf);
    //重新创建一个以filename命名的文件
    return reg;
	*/
}

int read_conf(const char *filename/*in*/, const char **key/*in*/, int cnt/*in*/, struct hb_conf *conf){

    FILE *pf = NULL;
    char line[1024] = {0}, vtemp[1024] = {0};

    pf = fopen(filename, "r"); //以只读方式打开
	if(pf == NULL)
	{
		printf("read conf file failed\n");
	    return 1;
	}
    
    while(fgets(line, 1024, pf)!=NULL){
        getValue(line, key, cnt, conf);
		if(line == "\n" || line == " ")
		  break;
    }
    if(pf != NULL)
        fclose(pf);

	return 0;
}
