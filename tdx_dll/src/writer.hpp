#pragma once

class logger
{
private:
    std::string log_file;
    std::ofstream *_file;

public:
    logger()
    {
        // 获取 ~目录
        char *home = getenv("USERPROFILE");

        // 日志 tdx_dll.log
        log_file = std::string(home) + "\\tdx_dll.log";

        // 打开文件
        _file = new std::ofstream(log_file.data());

        *_file << "logger 初始化"
               << "\n";

        _file->close();
    }

    void log(std::string msg)
    {
        _file->open(log_file.data(), std::ios::out | std::ios::app);
        *_file << msg << "\n";
        _file->close();
    }

    ~logger()
    {
        if (_file)
        {
            _file->close();
            delete _file;
        }
    }
};

class writer
{
protected:
    // 日志
    logger log;

    // 输出流
    std::map<int, std::ofstream *> _files;

    // 输出计数
    std::map<int, int> _files_count;

    // 当前输出流
    std::ofstream *_file;

    // 当前code
    int _code;

    // 要写入的数据长度
    int _length;

    // 当前长度
    int _cur_length;

    // 日期文件夹
    std::string date_folder;

    void get_cur_date(std::string &_date)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm *date = std::localtime(&time);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", date);

        _date = std::string(buffer);
    }

    // 打开文件
    void init_file(int code)
    {
        // 关闭之前的文件
        if ((_files[code]) && (_files[code]->is_open()))
        {
            _files[code]->close();
            delete _files[code];
        }

        std ::string file_name = date_folder + std::to_string(code) + ".csv";
        _files[code] = new std::ofstream(file_name.data(), std::ios::out | std::ios::app);
        _files_count[code] = 0;
        _file = _files[code];
    }

public:
    writer()
    {
        log.log("初始化");

        // 获取当前 DLL 的模块句柄
        HMODULE hDllModule = GetModuleHandle(NULL);
        log.log(std::string("dll 句柄: ") + std::to_string((int)hDllModule));

        // 获取dll完整路径
        char path[MAX_PATH];
        GetModuleFileName(hDllModule, path, MAX_PATH);
        log.log(std::string("dll路径: ") + path);

        // 读取配置文件 .dll -> .text
        std::string config_file = std::string(path).replace(std::string(path).rfind("tdxw.exe"), 8, "T0002\\dlls\\dll_config.txt");
        log.log(std::string("config_file: ") + config_file);

        // 默认路径
        std::string data_path = std::string(path).replace(std::string(path).rfind("tdxw.exe"), 8, "T0002\\dlls\\data\\");
        log.log(std::string("默认数据路径: ") + data_path);

        int user_data_path = 0;
        // 配置路径
        if (_access(config_file.data(), 0) != -1)
        {
            std::string temp_data_path;
            std::ifstream config(config_file.data());

            // 读取第一行
            std::getline(config, temp_data_path);
            log.log(std::string("配置数据路径: ") + temp_data_path);

            // 如果不是 '\' 结尾,则添加
            if (temp_data_path[temp_data_path.length() - 1] != '\\')
            {
                temp_data_path += '\\';
                log.log(std::string("修正路径: ") + temp_data_path);
            }

            if (_access(data_path.data(), 0) != -1)
            {
                data_path = temp_data_path;
                log.log(std::string("数据路径: ") + data_path);
                user_data_path = 1;
            }
        }

        if (user_data_path == 0)
        {
            log.log("使用默认数据路径");
            if (_access(data_path.data(), 0) == -1)
            {
                log.log(std::string("新建数据件夹: ") + data_path);
                _mkdir(data_path.data());
            }
        }
        else
        {
            log.log("使用配置数据路径");
        }

        // 初始化日期文件夹
        std::string date;
        get_cur_date(date);
        log.log(std::string("日期: ") + date);
        date_folder = data_path + date + "\\";
        if (_access(date_folder.data(), 0) == -1)
        {
            log.log(std::string("新建日期文件夹: ") + date_folder);
            _mkdir(date_folder.data());
        }

        log.log("初始化完成");
    }

    ~writer()
    {
        for (auto &file : _files)
        {
            if (file.second)
            {
                if (file.second->is_open())
                {
                    file.second->close();
                }

                delete file.second;
            }
        }
    }

    // 切换输出文件
    void switch_code(int code, int time, int length)
    {
        // log.log(std::string("切换code: ") + std::to_string(code));
        // log.log(std::string("time: ") + std::to_string(time));
        // log.log(std::string("length: ") + std::to_string(length));

        if (_files.find(code) == _files.end())
        {
            // log.log("初始化文件");
            init_file(code);
        }

        // 切换
        _code = code;
        _length = length;
        _cur_length = 0;

        // 写时间
        *_file << time;
    }

    void write(float data)
    {
        if (_file)
        {
            if (_cur_length + 1 <= _length)
            {
                // log.log(std::string("写入数据: ") + std::to_string(data));

                // 写数据
                *_file << "," << data;

                // 计数
                _cur_length++;
                _files_count[_code]++;

                if (_cur_length == _length)
                {
                    // log.log("数据写满,结束数据行");
                    *_file << "\n";
                }
            }
        }
    }
};