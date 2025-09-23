log:=utils/logc/log.h utils/logc/log.c
mqtt:=app/app_mqtt.h app/app_mqtt.c
cjson:=utils/cJSON/cJSON.h utils/cJSON/cJSON.c
app_common:=app/app_common.c app/app_common.h

logtest: test/logtest.c $(log)
	-$(CC) -o $@ $^ -Iutils
	-./$@
	-rm ./$@


cjsontest: test/cJSONtest.c $(cjson)
	-$(CC) -o $@ $^ -Iutils
	-./$@
	-rm ./$@

mqtttest: test/mqtttest.c $(log) $(mqtt)
	-$(CC) -o $@ $^ -Iutils -lpaho-mqtt3c
	-sudo ./$@
	-rm ./$@

app_common_test: test/app_common_test.c $(app_common)
	-$(CC) -o $@ $^ -Iapp
	-./$@
	-rm ./$@

buffer_test: test/app_buffer_test.c $(log)
	-$(CC) -o $@ $^ -Iapp
	-sudo ./$@
	-rm ./$@
