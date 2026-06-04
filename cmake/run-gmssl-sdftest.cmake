if(NOT DEFINED SOFTSDFINIT)
	message(FATAL_ERROR "SOFTSDFINIT is not set")
endif()
if(NOT DEFINED SOFTSDF_LIBRARY)
	message(FATAL_ERROR "SOFTSDF_LIBRARY is not set")
endif()
if(NOT DEFINED GMSSL_EXECUTABLE)
	message(FATAL_ERROR "GMSSL_EXECUTABLE is not set")
endif()
if(NOT DEFINED WORK_DIR)
	message(FATAL_ERROR "WORK_DIR is not set")
endif()

file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")

set(test_pass "P@ssw0rd")

execute_process(
	COMMAND "${SOFTSDFINIT}" -kek 1 -key 1 -pass "${test_pass}"
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE init_result)
if(NOT init_result EQUAL 0)
	message(FATAL_ERROR "softsdfinit failed with exit code ${init_result}")
endif()

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sdfinfo -lib "${SOFTSDF_LIBRARY}"
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sdfinfo_result)
if(NOT sdfinfo_result EQUAL 0)
	message(FATAL_ERROR "gmssl sdfinfo failed with exit code ${sdfinfo_result}")
endif()

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sdfdigest -lib "${SOFTSDF_LIBRARY}" -in_str abc -out sdfdigest.txt
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sdfdigest_result)
if(NOT sdfdigest_result EQUAL 0)
	message(FATAL_ERROR "gmssl sdfdigest failed with exit code ${sdfdigest_result}")
endif()

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sm3 -in_str abc -out sm3.txt
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sm3_result)
if(NOT sm3_result EQUAL 0)
	message(FATAL_ERROR "gmssl sm3 failed with exit code ${sm3_result}")
endif()

file(READ "${WORK_DIR}/sdfdigest.txt" sdfdigest)
file(READ "${WORK_DIR}/sm3.txt" sm3digest)
string(STRIP "${sdfdigest}" sdfdigest)
string(STRIP "${sm3digest}" sm3digest)
if(NOT sdfdigest STREQUAL sm3digest)
	message(FATAL_ERROR "SDF digest mismatch: ${sdfdigest} != ${sm3digest}")
endif()

file(WRITE "${WORK_DIR}/message.txt" "message to be signed and encrypted")

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sdfexport -lib "${SOFTSDF_LIBRARY}" -sign -key 1 -out sm2signpub.pem
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sdfexport_sign_result)
if(NOT sdfexport_sign_result EQUAL 0)
	message(FATAL_ERROR "gmssl sdfexport -sign failed with exit code ${sdfexport_sign_result}")
endif()

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sdfsign -lib "${SOFTSDF_LIBRARY}" -key 1 -pass "${test_pass}" -in message.txt -out message.sig
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sdfsign_result)
if(NOT sdfsign_result EQUAL 0)
	message(FATAL_ERROR "gmssl sdfsign failed with exit code ${sdfsign_result}")
endif()

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sm2verify -pubkey sm2signpub.pem -in message.txt -sig message.sig
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sm2verify_result)
if(NOT sm2verify_result EQUAL 0)
	message(FATAL_ERROR "gmssl sm2verify failed with exit code ${sm2verify_result}")
endif()

execute_process(
	COMMAND "${GMSSL_EXECUTABLE}" sdfexport -lib "${SOFTSDF_LIBRARY}" -encrypt -key 1 -out sm2encpub.pem
	WORKING_DIRECTORY "${WORK_DIR}"
	RESULT_VARIABLE sdfexport_encrypt_result)
if(NOT sdfexport_encrypt_result EQUAL 0)
	message(FATAL_ERROR "gmssl sdfexport -encrypt failed with exit code ${sdfexport_encrypt_result}")
endif()
