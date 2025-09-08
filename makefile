log:=utils/logc/log.h utils/logc/log.c

logtest: test/logtest.c $(log)
	-$(CC) -o $@ $^ -Iutils
	-./$@
	-rm ./$@

cjson:=utils/cJSON/cJSON.h utils/cJSON/cJSON.c

cjsontest: test/cJSONtest.c $(cjson)
	-$(CC) -o $@ $^ -Iutils
	-./$@
	-rm ./$@

	
