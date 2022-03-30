#include <script.h>

Script::Script()
	: m_pHandler(nullptr) {
	/// clear cmd result
	memset(m_cCmdRes, 0, MAX_SCRIPT_RES_SIZE);
}

Script::~Script() {
}

const char* Script::executeCMD(const char* pCmd) {
	/// clear cmd result
	memset(m_cCmdRes, 0, MAX_SCRIPT_RES_SIZE);
	
	/// copy cmd
	memcpy(m_cCmdRes, pCmd, strlen(pCmd));
	memcpy(m_cCmdRes + strlen(pCmd), CMD_RES, strlen(CMD_RES));

	/// execute
	if ((m_pHandler = popen(m_cCmdRes, "r"))) {
		fgets(m_cCmdRes, sizeof(m_cCmdRes), m_pHandler);
		pclose(m_pHandler);
	}
	
	/// return result
	return m_cCmdRes;
}