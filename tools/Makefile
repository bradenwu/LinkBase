Base64Objects:=base64_pb_to_json.o  ../cpp/linkbase.pb.o qbase64.o 
DeleteExpiredDeadObjects:=../cpp/linkbase.pb.o qbase64.o delete_expired_dead_url.o
CXX=g++
CXXFLAGS=-g 
INCLUDE=-I ../cpp
LDFLAGS=-l protobuf

#SOURCES = $(wildcard *.cpp)
# 静态模式 + 模式规则
%.o : %.cpp 
	       $(CXX) $(CXXFLAGS) -c $(INCLUDE) -o $@ $<
%.o : %.cc
	       $(CXX) $(CXXFLAGS) -c $(INCLUDE) -o $@ $<
%.o : %.cxx
	       $(CXX) $(CXXFLAGS) -c $(INCLUDE) -o $@ $<
.PHONY:all clean
all:base64_pb_to_json delete_expired_dead_url 

base64_pb_to_json: $(Base64Objects)
	@echo "开始编译转换工具..."
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
	@echo "编译转换工具 完成!"
delete_expired_dead_url: $(DeleteExpiredDeadObjects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
clean:
	@echo "开始清理所有编译生成的数据..."
	rm -rfv *.o ../cpp/linkbase.pb.o base64_pb_to_json delete_expired_dead_url
	@echo "done 清理所有编译生成的数据."
