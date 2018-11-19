#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <fstream>

struct bulk_constructor{
    bulk_constructor(size_t bulk_size): size(bulk_size) {}
    void add_subscriber(std::function<void(int64_t,const std::vector<std::string>&)> new_fun){
        subscribers.push_back(new_fun);
    }
    bool process_char(char ch){
        bool ret = true;
        switch(ch){
            case '\n':
                if(state == constructor_state::DEFAULT){ 
                    state=constructor_state::FINISH_INPUT;
                }else{
                    state=constructor_state::DEFAULT;
                }
                break;
            case '{':
                if(state == constructor_state::DEFAULT){ 
                    state=constructor_state::OPEN_BRACKET;
                }
                break;
            case '}':
                if(state == constructor_state::DEFAULT){
                    state=constructor_state::CLOSE_BRACKET;
                }
                break;
            case -1:
                state=constructor_state::FINISH_STREAM;
                break;
            default: 
                state = constructor_state::INPUT;                
        }
        switch(state){
             case constructor_state::DEFAULT:
                if(input_stream.tellp()!=0){                    
                    bulk_element.push_back(input_stream.str());
                    if( bulk_element.size() >= size && !opened_brackets){
                        print();
                        set_defaults();
                    }
                    input_stream.str(std::string{});
                    if(timestamp==0){
                        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    }
                }
                break;
             case constructor_state::OPEN_BRACKET:
                opened_brackets++;
                break;
             case constructor_state::CLOSE_BRACKET:
                opened_brackets--;
                if(opened_brackets == 0){
                    print();
                    set_defaults();
                }
                if(opened_brackets < 0){
                    set_defaults();
                }
                break;
             case constructor_state::INPUT:
                input_stream<<ch;
                break;
             case constructor_state::FINISH_STREAM:
                ret = false;
             case constructor_state::FINISH_INPUT:
                if( opened_brackets == 0 ){                
                    print();
                    set_defaults();
                }else{
                    set_defaults();
                }
                break;             
        }
        
        return ret;
    }
private:
    enum class constructor_state{
        DEFAULT,
        INPUT,
        OPEN_BRACKET,
        CLOSE_BRACKET,
        FINISH_INPUT,
        FINISH_STREAM
    };
    std::stringstream input_stream;
    constructor_state state = constructor_state::DEFAULT; 
    std::vector<std::string> bulk_element;
    size_t size;
    int opened_brackets = 0;
    void print(){
        for(auto& fn: subscribers){
            fn(timestamp,bulk_element);
        }
    }

    void set_defaults(){
        opened_brackets = 0;
        bulk_element.clear();
        timestamp = 0;
    }

    int64_t timestamp=0;
    std::vector<std::function<void(int64_t,const std::vector<std::string>&)>> subscribers;
};


void screen_print(int64_t, const std::vector<std::string>& commands){
    std::cout<<"bulk:";
    for(auto& cmd: commands){
        std::cout<<' '<<cmd;
    }
    std::cout<<'\n';
}

void log_print(int64_t timestamp, const std::vector<std::string>& commands){
    std::ofstream logfile;
    std::stringstream filename;
    filename<<"bulk";
    filename<<timestamp;
    filename<<".log";
    logfile.open(filename.str());
    for(auto& cmd: commands){
        logfile<<cmd<<'\n';
    }
    logfile.close();
}

int main(int, char** argw){
    std::stringstream ss;
    ss<<argw[1];
    size_t bulk_size;
    ss>>bulk_size;
    
    bulk_constructor constructor{bulk_size};
    constructor.add_subscriber(screen_print);
    constructor.add_subscriber(log_print);
    while(constructor.process_char(std::getchar())){
        
    }
    return 0;
}