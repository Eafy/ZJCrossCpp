/*******************************************************************************
 * Project:  neb
 * @file     CJsonObject.cpp
 * @brief 
 * @author   bwarliao
 * @date:    2014-7-16
 * @note
 * Modify history:
 ******************************************************************************/

#include "CJsonObject.hpp"

namespace neb
{

CJsonObject::CJsonObject()
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    // m_pJsonData = cJSONZJ_CreateObject();  
}

CJsonObject::CJsonObject(const std::string& strJson)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    Parse(strJson);
}

CJsonObject::CJsonObject(const CJsonObject* pJsonObject)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    if (pJsonObject)
    {
        Parse(pJsonObject->ToString());
    }
}

CJsonObject::CJsonObject(const CJsonObject& oJsonObject)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL)
{
    Parse(oJsonObject.ToString());
}

CJsonObject::~CJsonObject()
{
    Clear();
}

CJsonObject& CJsonObject::operator=(const CJsonObject& oJsonObject)
{
    Parse(oJsonObject.ToString().c_str());
    return(*this);
}

bool CJsonObject::operator==(const CJsonObject& oJsonObject) const
{
    return(this->ToString() == oJsonObject.ToString());
}

bool CJsonObject::AddEmptySubObject(const std::string& strKey)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateObject();
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("create sub empty object error!");
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::AddEmptySubArray(const std::string& strKey)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateArray();
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("create sub empty array error!");
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::GetKey(std::string& strKey)
{
    if (IsArray())
    {
        return(false);
    }
    if (m_listKeys.size() == 0)
    {
        cJSONZJ* pFocusData = NULL;
        if (m_pJsonData != NULL)
        {
            pFocusData = m_pJsonData;
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            pFocusData = m_pExternJsonDataRef;
        }
        else
        {
            return(false);
        }

        cJSONZJ*c = pFocusData->child;
        while (c)
        {
            m_listKeys.push_back(c->string);
            c = c->next;
        }
        m_itKey = m_listKeys.begin();
    }

    if (m_itKey == m_listKeys.end())
    {
        strKey = "";
        m_itKey = m_listKeys.begin();
        return(false);
    }
    else
    {
        strKey = *m_itKey;
        ++m_itKey;
        return(true);
    }
}

bool CJsonObject::HasKey(const std::string& strKey) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    
    return true;
}

int CJsonObject::Type(const std::string& strKey) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return -1;
    }
    
    return pJsonStruct->type;
}

CJsonObject& CJsonObject::operator[](const std::string& strKey)
{
    std::map<std::string, CJsonObject*>::iterator iter;
    iter = m_mapJsonObjectRef.find(strKey);
    if (iter == m_mapJsonObjectRef.end())
    {
        cJSONZJ* pJsonStruct = NULL;
        if (m_pJsonData != NULL)
        {
            if (m_pJsonData->type == cJSONZJ_Object)
            {
                pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
            }
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            if (m_pExternJsonDataRef->type == cJSONZJ_Object)
            {
                pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
            }
        }
        if (pJsonStruct == NULL)
        {
            CJsonObject* pJsonObject = new CJsonObject();
            m_mapJsonObjectRef.insert(std::pair<std::string, CJsonObject*>(strKey, pJsonObject));
            return(*pJsonObject);
        }
        else
        {
            CJsonObject* pJsonObject = new CJsonObject(pJsonStruct);
            m_mapJsonObjectRef.insert(std::pair<std::string, CJsonObject*>(strKey, pJsonObject));
            return(*pJsonObject);
        }
    }
    else
    {
        return(*(iter->second));
    }
}

CJsonObject& CJsonObject::operator[](unsigned int uiWhich)
{
    std::map<unsigned int, CJsonObject*>::iterator iter;
    iter = m_mapJsonArrayRef.find(uiWhich);
    if (iter == m_mapJsonArrayRef.end())
    {
        cJSONZJ* pJsonStruct = NULL;
        if (m_pJsonData != NULL)
        {
            if (m_pJsonData->type == cJSONZJ_Array)
            {
                pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, uiWhich);
            }
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            if (m_pExternJsonDataRef->type == cJSONZJ_Array)
            {
                pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, uiWhich);
            }
        }
        if (pJsonStruct == NULL)
        {
            CJsonObject* pJsonObject = new CJsonObject();
            m_mapJsonArrayRef.insert(std::pair<unsigned int, CJsonObject*>(uiWhich, pJsonObject));
            return(*pJsonObject);
        }
        else
        {
            CJsonObject* pJsonObject = new CJsonObject(pJsonStruct);
            m_mapJsonArrayRef.insert(std::pair<unsigned int, CJsonObject*>(uiWhich, pJsonObject));
            return(*pJsonObject);
        }
    }
    else
    {
        return(*(iter->second));
    }
}

std::string CJsonObject::operator()(const std::string& strKey) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(std::string(""));
    }
    if (pJsonStruct->type == cJSONZJ_String)
    {
        return(pJsonStruct->valuestring);
    }
    else if (pJsonStruct->type == cJSONZJ_Int)
    {
        char szNumber[128] = {0};
        if (pJsonStruct->sign == -1)
        {
            if ((int64)pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
            {
                snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
            }
        }
        else
        {
            if (pJsonStruct->valueint <= (uint64)UINT_MAX)
            {
                snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
            }
        }
        return(std::string(szNumber));
    }
    else if (pJsonStruct->type == cJSONZJ_Double)
    {
        char szNumber[128] = {0};
        if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
        {
            snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
        }
        else
        {
            snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
        }
    }
    else if (pJsonStruct->type == cJSONZJ_False)
    {
        return(std::string("false"));
    }
    else if (pJsonStruct->type == cJSONZJ_True)
    {
        return(std::string("true"));
    }
    return(std::string(""));
}

std::string CJsonObject::operator()(unsigned int uiWhich) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, uiWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, uiWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(std::string(""));
    }
    if (pJsonStruct->type == cJSONZJ_String)
    {
        return(pJsonStruct->valuestring);
    }
    else if (pJsonStruct->type == cJSONZJ_Int)
    {
        char szNumber[128] = {0};
        if (pJsonStruct->sign == -1)
        {
            if ((int64)pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
            {
                snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
            }
        }
        else
        {
            if (pJsonStruct->valueint <= (uint64)UINT_MAX)
            {
                snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
            }
        }
        return(std::string(szNumber));
    }
    else if (pJsonStruct->type == cJSONZJ_Double)
    {
        char szNumber[128] = {0};
        if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
        {
            snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
        }
        else
        {
            snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
        }
    }
    else if (pJsonStruct->type == cJSONZJ_False)
    {
        return(std::string("false"));
    }
    else if (pJsonStruct->type == cJSONZJ_True)
    {
        return(std::string("true"));
    }
    return(std::string(""));
}

bool CJsonObject::Parse(const std::string& strJson)
{
    Clear();
    m_pJsonData = cJSONZJ_Parse(strJson.c_str());
    if (m_pJsonData == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSONZJ_GetErrorPtr();
        return(false);
    }
    return(true);
}

void CJsonObject::Clear()
{
    m_pExternJsonDataRef = NULL;
    if (m_pJsonData != NULL)
    {
        cJSONZJ_Delete(m_pJsonData);
        m_pJsonData = NULL;
    }
    for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
    }
    m_mapJsonArrayRef.clear();
    for (std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.begin();
                    iter != m_mapJsonObjectRef.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
    }
    m_mapJsonObjectRef.clear();
    m_listKeys.clear();
}

bool CJsonObject::IsEmpty() const
{
    if (m_pJsonData != NULL)
    {
        return(false);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::IsArray() const
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }

    if (pFocusData == NULL)
    {
        return(false);
    }

    if (pFocusData->type == cJSONZJ_Array)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool CJsonObject::IsDictionary() const
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    
    if (pFocusData == NULL)
    {
        return(false);
    }
    
    if (pFocusData->type == cJSONZJ_Object)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

std::string CJsonObject::ToString() const
{
    char* pJsonString = NULL;
    std::string strJsonData = "";
    if (m_pJsonData != NULL)
    {
        pJsonString = cJSONZJ_PrintUnformatted(m_pJsonData);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pJsonString = cJSONZJ_PrintUnformatted(m_pExternJsonDataRef);
    }
    if (pJsonString != NULL)
    {
        strJsonData = pJsonString;
        free(pJsonString);
    }
    return(strJsonData);
}

std::string CJsonObject::ToFormattedString() const
{
    char* pJsonString = NULL;
    std::string strJsonData = "";
    if (m_pJsonData != NULL)
    {
        pJsonString = cJSONZJ_Print(m_pJsonData);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pJsonString = cJSONZJ_Print(m_pExternJsonDataRef);
    }
    if (pJsonString != NULL)
    {
        strJsonData = pJsonString;
        free(pJsonString);
    }
    return(strJsonData);
}

bool CJsonObject::Get(const std::string& strKey, CJsonObject& oJsonObject) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    char* pJsonString = cJSONZJ_Print(pJsonStruct);
    std::string strJsonData = pJsonString;
    free(pJsonString);
    if (oJsonObject.Parse(strJsonData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool CJsonObject::Get(const std::string& strKey, std::string& strValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_String)
    {
        return(false);
    }
    strValue = pJsonStruct->valuestring;
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, int32& iValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    iValue = (int32)(pJsonStruct->valueint);
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, uint32& uiValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    uiValue = (uint32)(pJsonStruct->valueint);
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, int64& llValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    llValue = (int64)pJsonStruct->valueint;
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, uint64& ullValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    ullValue = (uint64)pJsonStruct->valueint;
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, bool& bValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type > cJSONZJ_True)
    {
        return(false);
    }
    bValue = pJsonStruct->type;
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, float& fValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Double)
    {
        return(false);
    }
    fValue = (float)(pJsonStruct->valuedouble);
    return(true);
}

bool CJsonObject::Get(const std::string& strKey, double& dValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Object)
        {
            pJsonStruct = cJSONZJ_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Double)
    {
        return(false);
    }
    dValue = pJsonStruct->valuedouble;
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, const CJsonObject& oJsonObject)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSONZJ_GetErrorPtr();
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, const std::string& strValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, int32 iValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, uint32 uiValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, int64 llValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, uint64 ullValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt(ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, bool bValue, bool bValueAgain)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, float fValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(const std::string& strKey, double dValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateObject();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Add(std::map<std::string,std::string> &map)
{
    for (std::map<std::string,std::string>::iterator iter = map.begin(); iter != map.end(); ++iter) {
        if (!this->Add(iter->first, iter->second)) {
            return false;
        }
    }

    return true;
}

bool CJsonObject::Delete(const std::string& strKey)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ_DeleteItemFromObject(pFocusData, strKey.c_str());
    std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    m_listKeys.clear();
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, const CJsonObject& oJsonObject)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSONZJ_GetErrorPtr();
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, const std::string& strValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, int32 iValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, uint32 uiValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, int64 llValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, uint64 ullValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, bool bValue, bool bValueAgain)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, float fValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(const std::string& strKey, double dValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSONZJ_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

int CJsonObject::GetArraySize()
{
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            return(cJSONZJ_GetArraySize(m_pJsonData));
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            return(cJSONZJ_GetArraySize(m_pExternJsonDataRef));
        }
    }
    return(0);
}

bool CJsonObject::Get(int iWhich, CJsonObject& oJsonObject) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    char* pJsonString = cJSONZJ_Print(pJsonStruct);
    std::string strJsonData = pJsonString;
    free(pJsonString);
    if (oJsonObject.Parse(strJsonData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool CJsonObject::Get(int iWhich, std::string& strValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_String)
    {
        return(false);
    }
    strValue = pJsonStruct->valuestring;
    return(true);
}

bool CJsonObject::Get(int iWhich, int32& iValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    iValue = (int32)(pJsonStruct->valueint);
    return(true);
}

bool CJsonObject::Get(int iWhich, uint32& uiValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    uiValue = (uint32)(pJsonStruct->valueint);
    return(true);
}

bool CJsonObject::Get(int iWhich, int64& llValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    llValue = (int64)pJsonStruct->valueint;
    return(true);
}

bool CJsonObject::Get(int iWhich, uint64& ullValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Int)
    {
        return(false);
    }
    ullValue = (uint64)pJsonStruct->valueint;
    return(true);
}

bool CJsonObject::Get(int iWhich, bool& bValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type > cJSONZJ_True)
    {
        return(false);
    }
    bValue = pJsonStruct->type == cJSONZJ_True;
    return(true);
}

bool CJsonObject::Get(int iWhich, float& fValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Double)
    {
        return(false);
    }
    fValue = (float)(pJsonStruct->valuedouble);
    return(true);
}

bool CJsonObject::Get(int iWhich, double& dValue) const
{
    cJSONZJ* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSONZJ_Array)
        {
            pJsonStruct = cJSONZJ_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSONZJ_Double)
    {
        return(false);
    }
    dValue = pJsonStruct->valuedouble;
    return(true);
}

bool CJsonObject::Add(const CJsonObject& oJsonObject)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSONZJ_GetErrorPtr();
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    unsigned int uiLastIndex = (unsigned int)cJSONZJ_GetArraySize(pFocusData) - 1;
    for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->first >= uiLastIndex)
        {
            if (iter->second != NULL)
            {
                delete (iter->second);
                iter->second = NULL;
            }
            m_mapJsonArrayRef.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    return(true);
}

bool CJsonObject::Add(const std::string& strValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(int32 iValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(uint32 uiValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(int64 llValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(uint64 ullValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(bool bValue)
{
    return AddBool(bValue);
}

bool CJsonObject::AddBool(bool bValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(float fValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Add(double dValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(const CJsonObject& oJsonObject)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSONZJ_GetErrorPtr();
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter++);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(const std::string& strValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(int32 iValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(uint32 uiValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)uiValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(int64 llValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(uint64 ullValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)ullValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(bool bValue, bool bValueAgain)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(float fValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::AddAsFirst(double dValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSONZJ_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSONZJ_GetArraySize(pFocusData);
    cJSONZJ_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSONZJ_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Delete(int iWhich)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ_DeleteItemFromArray(pFocusData, iWhich);
    for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->first >= (unsigned int)iWhich)
        {
            if (iter->second != NULL)
            {
                delete (iter->second);
                iter->second = NULL;
            }
            m_mapJsonArrayRef.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, const CJsonObject& oJsonObject)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSONZJ_GetErrorPtr();
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, const std::string& strValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, int32 iValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, uint32 uiValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, int64 llValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)((uint64)llValue), -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, uint64 ullValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, bool bValue, bool bValueAgain)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, float fValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool CJsonObject::Replace(int iWhich, double dValue)
{
    cJSONZJ* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSONZJ_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSONZJ* pJsonStruct = cJSONZJ_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSONZJ_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSONZJ_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

CJsonObject::CJsonObject(cJSONZJ* pJsonData)
    : m_pJsonData(NULL), m_pExternJsonDataRef(pJsonData)
{
}

}


