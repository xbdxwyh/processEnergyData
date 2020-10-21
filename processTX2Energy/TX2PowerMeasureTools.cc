/*
*The delay of using the script to measure the power consumption is too large. 
*The script sets the sleep to record once for 0.05 seconds, and then outputs once for one second 
*(that is, twenty times). Compared with the actual one second, the time used by this 
*method of the script is about 8-9, which further increases the sampling frequency but does not 
*improve the effect. The YOLO series uses GPU acceleration in TX2 for less than 1s. Using scripts 
*will miss the peak of GPU power consumption and cause considerable errors. 
*So in order to be able to read the power consumption within a lower latency,
* this code is written to count the power consumption.


First, explain why the script is not accurate. In the Jeston forum, 
 someone pointed out that the overhead caused by the script to open and close the file is too large,
 and the cat command is used to view the content in the script. One person used the C language to read the information.
 I copied his code and made "a little change".
 
/////////////////////////https://forums.developer.nvidia.com/t/jetson-tx2-ina226-power-monitor-with-i2c-interface/48754/4

For the file used in the script, I did not confirm whether it is a file representing this physical value,
	so if you find an error, please modify it in time.
	

At the same time, I observed that the more physical items recorded at the same time, 
	the slower the program, and the more recorded values (that is, the more average),
	the slower the program. Fortunately, using C/C++ only needs to open the file once,
	so it can be much faster than the cat instruction. In addition, 
	you can use g++ -O3 pwMointor.cc -o pwmointorwhen compiling;
	use the -O3 option to optimize the program to make it faster.

Finally, use the macro definition to determine the physical item you need to measure. 
	If you want to find the voltage or current, please search the file name on the Internet, 
	and then use it in this form.


The data will eventually be saved in csv format, with "," as the separator,
	time represents the time consumed by the average of the Freq values tested, 
	please note that this is not the current timestamp.

The data will eventually be saved in csv format, with "," as the separator, 
	time represents the time consumed by the average of the Freq values tested, 
	please note that this is not the current timestamp.

In the end, our program can record that TX2 has a power peak of more than 8W when using GPU calculations,
	which is close to the value of TX2 rated power of 7.5W in the manual. The script cannot do this.
	In my record, it only A peak value of no more than 5W can be recorded.

Originally, this program was in pure C language, but it may not be output to the file correctly 
	due to problems with the use of the fprintf function, so I used the c++ stream file in 
	the output part of the file.

*/



#include <stdlib.h>
#include <errno.h>

#include <thread>

#include <fstream>
#include <iostream>

#include <chrono>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>

const long long sleepNanoTime = 1e7;

const int Freq = 200;

inline long long getTimeStampMill(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline long long getTimeStampNano(){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline void sleepNanoSeconds(long long sleepTime){
    std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
}

#define A7W_TEST

/*
#define CPU_TEST
#define DDR_TEST

#define GPU_TEST

#define SOC_TEST
#define WIFI_TEST
#define MUX_TEST
#define IO_TEST
#define SYS_TEST
*/
int InstantPowerSampling(long long sleepTime){
    //std::cout << "InstantPowerSampling" << std::endl;
    
    std::ofstream logfile;
	logfile.open("log.csv",std::ios::out);
	
	std::ios::sync_with_stdio(false);
	
	logfile << "timeStamp";
	
	#ifdef A7W_TEST
    int A7_W_fd     =   open("/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/in_power0_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0041/iio_device/in_power0_input) failed!\n");
        exit(1);
    }
    logfile << ",total_power(mW)";
    #endif
	
	#ifdef CPU_TEST
    int CPU_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/in_power1_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0041/iio_device/in_power1_input) failed!\n");
        exit(1);
    }
    logfile << ",cpu_power(mW)";
    #endif
	
	#ifdef DDR_TEST
    int DDR_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/in_power2_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0041/iio_device/in_power2_input) failed!\n");
        exit(1);
    }
    logfile << ",ddr_power(mW)";
    #endif
	
	#ifdef GPU_TEST
    int GPU_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device/in_power0_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0040/iio_device/in_power0_input) failed!\n");
        exit(1);
    }
    logfile << ",gpu_power(mW)";
    #endif
	
	#ifdef SOC_TEST
    int SOC_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device/in_power1_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0040/iio_device/in_power1_input) failed!\n");
        exit(1);
    }
    logfile << ",soc_power(mW)";
	#endif
    
	#ifdef WIFI_TEST
    int WIFI_fd     =   open("/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device/in_power2_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0040/iio_device/in_power2_input) failed!\n");
        exit(1);
    }
    logfile << ",wifi_power(mW)";
    #endif
	
	#ifdef MUX_TEST
    int MUX_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device/in_power0_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0042/iio_device/in_power0_input) failed!\n");
        exit(1);
    }
    logfile << ",mux_power(mW)";
	#endif
    
	#ifdef IO_TEST
    int IO_fd       =   open("/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device/in_power1_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0042/iio_device/in_power1_input) failed!\n");
        exit(1);
    }
    logfile << ",io_power(mW)";
	#endif
    
	#ifdef SYS_TEST
    int SYS_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device/in_power2_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0042/iio_device/in_power2_input) failed!\n");
        exit(1);
    }
    logfile << ",sys_power(mW)";
	#endif
	
	logfile << ",costTime(ms)" << std::endl;
	
    char *noUseOut = NULL;
    double sum = 0;
    long long startTime;
    
    double start = 0;
    
    char A7_W_buf[32],CPU_buf[32],DDR_buf[32],GPU_buf[32],SOC_buf[32],WIFI_buf[32],MUX_buf[32],IO_buf[32],SYS_buf[32];
    
    while (1) {

        startTime = getTimeStampNano();
        
        sleepNanoSeconds(sleepTime);		
        
        logfile << getTimeStampMill();
        
        #ifdef A7W_TEST
        lseek(A7_W_fd, 0, 0);
		int readA7WInfoFlag     =   read(A7_W_fd, A7_W_buf, 32);
		if ( readA7WInfoFlag > 0 )  {
		    logfile << "," << strtod(A7_W_buf, &noUseOut);
        }
        #endif
        
		#ifdef CPU_TEST
        lseek(CPU_fd, 0, 0);
		int readCPUInfoFlag     =   read(CPU_fd, CPU_buf, 32);
		if ( readCPUInfoFlag > 0 )  {
            logfile << "," << strtod(CPU_buf, &noUseOut);
        }
		#endif
		
		#ifdef DDR_TEST
        lseek(DDR_fd, 0, 0);
		int readDDRInfoFlag     =   read(DDR_fd, DDR_buf, 32);
		if ( readDDRInfoFlag > 0 )  {
		    logfile << "," << strtod(DDR_buf, &noUseOut);
        }
        #endif
		
		#ifdef GPU_TEST
        lseek(GPU_fd, 0, 0);
		int readGPUInfoFlag     =   read(GPU_fd, GPU_buf, 32);
		if ( readGPUInfoFlag > 0 )  {
		    logfile << "," << strtod(GPU_buf, &noUseOut);
        }
		#endif
		
		#ifdef SOC_TEST
        lseek(SOC_fd, 0, 0);
		int readSOCInfoFlag     =   read(SOC_fd, SOC_buf, 32);
		if ( readSOCInfoFlag > 0 )  {
		    logfile << "," << strtod(SOC_buf, &noUseOut);
        }
		#endif
		
		#ifdef WIFI_TEST
        lseek(WIFI_fd, 0, 0);
		int readWIFIInfoFlag    =   read(WIFI_fd, WIFI_buf, 32);
		if ( readWIFIInfoFlag > 0 )  {
		    logfile << "," << strtod(WIFI_buf, &noUseOut);
        }
		#endif
		
		#ifdef MUX_TEST
        lseek(MUX_fd, 0, 0);
		int readMUXInfoFlag     =   read(MUX_fd, MUX_buf, 32);
		if ( readMUXInfoFlag > 0 )  {
		    logfile << "," << strtod(MUX_buf, &noUseOut);
        }
		#endif
		
		#ifdef IO_TEST
        lseek(IO_fd, 0, 0);
		int readIOInfoFlag      =   read(IO_fd, IO_buf, 32);
		if ( readIOInfoFlag > 0 )  {
		    logfile << "," << strtod(IO_buf, &noUseOut);
        }
		#endif
		
		#ifdef SYS_TEST
        lseek(SYS_fd, 0, 0);
		int readSYSInfoFlag     =   read(SYS_fd, SYS_buf, 32);
		if ( readSYSInfoFlag > 0 )  {
		    logfile << "," << strtod(SYS_buf, &noUseOut);
        }
		#endif

        logfile << "," << (getTimeStampNano() - startTime) * 1e-6 << std::endl;
        
    }
    
    return 1;
}

int PowerSampleByTimes(int sampleTimes){
    //std::cout << "PowerSampleByTimes" << std::endl;    
    
    std::ofstream logfile;
	logfile.open("log.csv",std::ios::out);
	
	std::ios::sync_with_stdio(false);
	
	logfile << "timeStamp";
	
	#ifdef A7W_TEST
    int A7_W_fd     =   open("/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/in_power0_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0041/iio_device/in_power0_input) failed!\n");
        exit(1);
    }
    logfile << ",total_power(mW)";
    #endif
	
	#ifdef CPU_TEST
    int CPU_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/in_power1_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0041/iio_device/in_power1_input) failed!\n");
        exit(1);
    }
    logfile << ",cpu_power(mW)";
    #endif
	
	#ifdef DDR_TEST
    int DDR_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/in_power2_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0041/iio_device/in_power2_input) failed!\n");
        exit(1);
    }
    logfile << ",ddr_power(mW)";
    #endif
	
	#ifdef GPU_TEST
    int GPU_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device/in_power0_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0040/iio_device/in_power0_input) failed!\n");
        exit(1);
    }
    logfile << ",gpu_power(mW)";
    #endif
	
	#ifdef SOC_TEST
    int SOC_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device/in_power1_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0040/iio_device/in_power1_input) failed!\n");
        exit(1);
    }
    logfile << ",soc_power(mW)";
	#endif
    
	#ifdef WIFI_TEST
    int WIFI_fd     =   open("/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device/in_power2_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0040/iio_device/in_power2_input) failed!\n");
        exit(1);
    }
    logfile << ",wifi_power(mW)";
    #endif
	
	#ifdef MUX_TEST
    int MUX_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device/in_power0_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0042/iio_device/in_power0_input) failed!\n");
        exit(1);
    }
    logfile << ",mux_power(mW)";
	#endif
    
	#ifdef IO_TEST
    int IO_fd       =   open("/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device/in_power1_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0042/iio_device/in_power1_input) failed!\n");
        exit(1);
    }
    logfile << ",io_power(mW)";
	#endif
    
	#ifdef SYS_TEST
    int SYS_fd      =   open("/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device/in_power2_input", O_RDONLY | O_NONBLOCK);
    if  ( A7_W_fd < 0 ) {
        perror("open(i2c-0/0-0042/iio_device/in_power2_input) failed!\n");
        exit(1);
    }
    logfile << ",sys_power(mW)";
	#endif
	
	logfile << ",costTime(ms)" << std::endl;
	
    int cnt = 0;
    char *noUseOut = NULL;
    double sum = 0;
    long long startTime;
    
    int A7W_cnt = 0,CPU_cnt = 0,DDR_cnt = 0,GPU_cnt = 0,SOC_cnt = 0,WIFI_cnt = 0,MUX_cnt = 0,IO_cnt = 0,SYS_cnt = 0;
    
    double A7W_sum = 0,CPU_sum = 0,DDR_sum = 0,GPU_sum = 0,SOC_sum = 0,WIFI_sum = 0,MUX_sum = 0,IO_sum = 0,SYS_sum = 0;
    double start = 0;
    
    char A7_W_buf[32],CPU_buf[32],DDR_buf[32],GPU_buf[32],SOC_buf[32],WIFI_buf[32],MUX_buf[32],IO_buf[32],SYS_buf[32];
    
    while (1) {
        if (cnt == 0) {
            startTime = getTimeStampNano();
        }
		
		#ifdef A7W_TEST
        lseek(A7_W_fd, 0, 0);
		int readA7WInfoFlag     =   read(A7_W_fd, A7_W_buf, 32);
		if ( readA7WInfoFlag > 0 )  {
            A7_W_buf[readA7WInfoFlag]   =   0;
            A7W_sum                     +=  strtod(A7_W_buf, &noUseOut);
            A7W_cnt                     +=  1;
            cnt                         +=  1;
        }
        #endif
		
		#ifdef CPU_TEST
        lseek(CPU_fd, 0, 0);
		int readCPUInfoFlag     =   read(CPU_fd, CPU_buf, 32);
		if ( readCPUInfoFlag > 0 )  {
            CPU_buf[readCPUInfoFlag]    =   0;
            CPU_sum                     +=  strtod(CPU_buf, &noUseOut);
            //cnt             +=  1;
			CPU_cnt						+=	1;
        }
		#endif
		
		#ifdef DDR_TEST
        lseek(DDR_fd, 0, 0);
		int readDDRInfoFlag     =   read(DDR_fd, DDR_buf, 32);
		if ( readDDRInfoFlag > 0 )  {
            DDR_buf[readDDRInfoFlag]    =   0;
            DDR_sum                     +=  strtod(DDR_buf, &noUseOut);
            //cnt             +=  1;
			DDR_cnt						+=	1;
        }
        #endif
		
		#ifdef GPU_TEST
        lseek(GPU_fd, 0, 0);
		int readGPUInfoFlag     =   read(GPU_fd, GPU_buf, 32);
		if ( readGPUInfoFlag > 0 )  {
            GPU_buf[readGPUInfoFlag]    =   0;
            GPU_sum                     +=  strtod(GPU_buf, &noUseOut);
            //cnt             +=  1;
			GPU_cnt						+=	1;
        }
		#endif
		
		#ifdef SOC_TEST
        lseek(SOC_fd, 0, 0);
		int readSOCInfoFlag     =   read(SOC_fd, SOC_buf, 32);
		if ( readSOCInfoFlag > 0 )  {
            SOC_buf[readSOCInfoFlag]    =   0;
            SOC_sum                     +=  strtod(SOC_buf, &noUseOut);
			SOC_cnt						+=	1;
            //cnt             +=  1;
        }
		#endif
		
		#ifdef WIFI_TEST
        lseek(WIFI_fd, 0, 0);
		int readWIFIInfoFlag    =   read(WIFI_fd, WIFI_buf, 32);
		if ( readWIFIInfoFlag > 0 )  {
            WIFI_buf[readWIFIInfoFlag]  =   0;
            WIFI_sum                    +=  strtod(WIFI_buf, &noUseOut);
			WIFI_cnt					+=	1;
            //cnt             +=  1;
        }
		#endif
		
		#ifdef MUX_TEST
        lseek(MUX_fd, 0, 0);
		int readMUXInfoFlag     =   read(MUX_fd, MUX_buf, 32);
		if ( readMUXInfoFlag > 0 )  {
            MUX_buf[readMUXInfoFlag]    =   0;
            MUX_sum                     +=  strtod(MUX_buf, &noUseOut);
			MUX_cnt						+=	1;
            //cnt             +=  1;
        }
		#endif
		
		#ifdef IO_TEST
        lseek(IO_fd, 0, 0);
		int readIOInfoFlag      =   read(IO_fd, IO_buf, 32);
		if ( readIOInfoFlag > 0 )  {
            IO_buf[readIOInfoFlag]       =   0;
            IO_sum                      +=  strtod(IO_buf, &noUseOut);
            //cnt             +=  1;
			IO_cnt						+=	1;
        }
		#endif
		
		#ifdef SYS_TEST
        lseek(SYS_fd, 0, 0);
		int readSYSInfoFlag     =   read(SYS_fd, SYS_buf, 32);
		if ( readSYSInfoFlag > 0 )  {
            SYS_buf[readSYSInfoFlag]    =   0;
            SYS_sum                     +=  strtod(SYS_buf, &noUseOut);
            //cnt             +=  1;
			SYS_cnt						+=	1;
        }
		#endif

        
        if (cnt >= sampleTimes) {
        /*
            struct timespec tv;
            clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
            double end = tv.tv_sec + tv.tv_nsec * 1e-9;
			double time = (end - start) * 1000;
	    */
			long long endTime = getTimeStampNano();
            //fprintf(stderr, "Read %d values in %.3f milliseconds\n", cnt, (end - start) * 1000);
            //fprintf(stderr, "average value was %.1f\n", A7W_sum / cnt);
            //fprintf(stderr, "average value was %.1f\n", CPU_sum / cnt);
            cnt = 0;
            sum = 0;
/*			
			//fprintf(logfp,"%lf",time);
			logfile << getTimeStamp() << "," << time;
			//std::cout << time;
			
			logfile << "," << (endTime - startTime) * 1e-6;
*/
			logfile << getTimeStampNano();
			
			#ifdef A7W_TEST
            //fprintf(logfp,",%lf",A7W_sum/A7W_cnt);
			logfile << "," << A7W_sum/A7W_cnt;
			//std::cout << "," << A7W_sum/A7W_cnt << std::endl;
            //printf("%f\n",A7W_sum/A7W_cnt);
            A7W_sum     =   0;
			A7W_cnt		=	0;
			#endif
			
			#ifdef CPU_TEST
			//fprintf(logfp,",%lf",CPU_sum/CPU_cnt);
			logfile << "," << CPU_sum/CPU_cnt;
            CPU_sum     =   0;
			CPU_cnt		=	0;
			#endif
			
			#ifdef DDR_TEST
			//fprintf(logfp,",%lf",DDR_sum/DDR_cnt);
			logfile << "," << DDR_sum/DDR_cnt;
            DDR_sum     =   0;
            DDR_cnt		=	0;
			#endif
			
			#ifdef GPU_TEST
			//fprintf(logfp,",%lf",GPU_sum/GPU_cnt);
			logfile << "," << GPU_sum/GPU_cnt;
			GPU_sum     =   0;
			GPU_cnt		=	0;
			#endif
			
			#ifdef SOC_TEST
			//fprintf(logfp,",%lf",SOC_sum/SOC_cnt);
			logfile << "," << SOC_sum/SOC_cnt;
            SOC_sum     =   0;
			SOC_cnt		=	0;
			#endif
			
			#ifdef WIFI_TEST
			//fprintf(logfp,",%lf",WIFI_sum/WIFI_cnt);
			logfile << "," <<WIFI_sum/WIFI_cnt;
            WIFI_sum    =   0;
			WIFI_cnt	=	0;
			#endif
			
			#ifdef MUX_TEST
			//fprintf(logfp,",%lf",MUX_sum/MUX_cnt);
			logfile << "," << MUX_sum/MUX_cnt;
            MUX_sum     =   0;
			MUX_cnt		=	0;
			#endif
			
			#ifdef IO_TEST
			logfile << "," << IO_sum/IO_cnt;
			//fprintf(logfp,",%lf",IO_sum/IO_cnt);
            IO_sum      =   0;
            IO_cnt		=	0;
			#endif
			
			#ifdef SYS_TEST
			logfile << "," << SYS_sum/SYS_cnt;
			//fprintf(logfp,",%lf",SYS_sum/SYS_cnt);
			SYS_sum     =   0;
            SYS_cnt		=	0;
			#endif
			
            cnt         =   0;
			
			//fprintf(logfp,"%s","\n");
			logfile << ","  << (endTime - startTime) * 1e-6 << std::endl;
        }
    }
    
    return 1;
}


int main(int argc,char** argv) {
    if (argc == 1){
        int flag = InstantPowerSampling(sleepNanoTime);
        //std::cout << flag << ",argc == 1" <<std::endl;
    } else if(argc == 2){
        //std::cout << argv[1] << std::endl;
        if (strcmp(argv[1] , "freq") == 0 || strcmp(argv[1] , "frequency") == 0){
            int flag = PowerSampleByTimes(Freq);
            //std::cout << flag << std::endl;
        } else if(strcmp(argv[1] , "itvl") == 0 || strcmp(argv[1] , "interval") == 0){
            int flag = InstantPowerSampling(sleepNanoTime);
            //std::cout << flag << std::endl;
        } else {
            std::cout << "ERROR,No Model Matched!" << std::endl;
        }
    } else {
        std::cout << "Too Many Arguments!" << std::endl;
    }
    return 0;
}


