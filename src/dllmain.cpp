#include "stdafx.h"
#include "helper.hpp"

using namespace std;

HMODULE baseModule = GetModuleHandle(NULL);

inipp::Ini<char> ini;

// INI Variables
bool bAspectFix;
bool bFOVFix;
bool bUncapFPS;
bool bDisableDOF;
int iCustomResX;
int iCustomResY;
int iInjectionDelay;
float fAdditionalFOV;
int iAspectFix;
int iFOVFix;

// Variables
float fNewX;
float fNewY;
float fNativeAspect = (float)16/9;
float fPi = 3.14159265358979323846f;
float fNewAspect;
string sFixVer = "1.0.5";

// CurrResolution Hook
DWORD64 CurrResolutionReturnJMP;
void __declspec(naked) CurrResolution_CC()
{
    __asm
    {
        mov r12d, r9d                               // Original code
        mov rbx, rdx                                // Original code
        mov rdi, [rax + r8 * 0x8]                   // Original code
        add rdi, rcx                                // Original code
        mov eax, [rdi]                              // Original code

        mov[iCustomResX], r15d                      // Grab current resX
        mov[iCustomResY], r12d                      // Grab current resY
        cvtsi2ss xmm14, r15d
        cvtsi2ss xmm15, r12d
        divss xmm14, xmm15
        movss[fNewAspect], xmm14                   // Grab current aspect ratio
        xorps xmm14, xmm14
        xorps xmm15, xmm15
        jmp[CurrResolutionReturnJMP]
    }
}

// Aspect Ratio/FOV Hook
DWORD64 AspectFOVFixReturnJMP;
float FOVPiDiv;
float FOVDivPi;
float FOVFinalValue;
void __declspec(naked) AspectFOVFix_CC()
{
    __asm
    {
        mov eax, [fNewAspect]                       // Move new aspect to eax
        cmp eax, [fNativeAspect]                    // Compare new aspect to native
        jle originalCode                            // Skip FOV fix if fNewAspect<=fNativeAspect
        cmp [iFOVFix], 1                            // Check if FOVFix is enabled
        je modifyFOV                                // jmp to FOV fix
        jmp originalCode                            // jmp to originalCode

        modifyFOV:
            fld dword ptr[rbx + 0x1F8]              // Push original FOV to FPU register st(0)
            fmul[FOVPiDiv]                          // Multiply st(0) by Pi/360
            fptan                                   // Get partial tangent. Store result in st(1). Store 1.0 in st(0)
            fxch st(1)                              // Swap st(1) to st(0)
            fdiv[fNativeAspect]                     // Divide st(0) by 1.778~
            fmul[fNewAspect]                        // Multiply st(0) by new aspect ratio
            fxch st(1)                              // Swap st(1) to st(0)
            fpatan                                  // Get partial arc tangent from st(0), st(1)
            fmul[FOVDivPi]                          // Multiply st(0) by 360/Pi
            fstp[FOVFinalValue]                     // Store st(0) 
            movss xmm0, [FOVFinalValue]             // Copy final FOV value to xmm0
            jmp originalCode

        originalCode:
            addss xmm0, [fAdditionalFOV]
            movss[rdi + 0x18], xmm0                 // Original code
            cmp [iAspectFix], 1
            je modifyAspect
            mov eax, [rbx + 0x00000208]             // Original code
            mov[rdi + 0x2C], eax                    // Original code
            jmp [AspectFOVFixReturnJMP]

        modifyAspect:
            mov eax, [fNewAspect]
            mov[rdi + 0x2C], eax                    // Original code
            jmp[AspectFOVFixReturnJMP]                         
    }
}

// UncapFPS Hook
DWORD64 UncapFPSReturnJMP;
void __declspec(naked) UncapFPS_CC()
{
    __asm
    {
        mov byte ptr[rbx+0x7A4], 0                  // bSmoothFrameRate = false
        jmp originalCode

        originalCode:
            movss xmm0, [rbx + 0x000007B8]          // Original code
            minss xmm0, xmm6                        // Original code
            movaps xmm6, xmm0                       // Original code
            jmp[UncapFPSReturnJMP]
    }
}

// FOV Culling Hook
DWORD64 FOVCullingReturnJMP;
float fOne = (float)1;
void __declspec(naked) FOVCulling_CC()
{
    __asm
    {
        movss xmm1, [fOne]                          //  r.StaticMeshLODDistanceScale | 1 = default, higher is worse
        jmp originalCode

        originalCode:
            movss[rdx + 0x000002E8], xmm1           // Original code
            movsd xmm0, [rbp + 0x000000E0]          // Original code
            jmp[FOVCullingReturnJMP]
    }
}

// HUD Markers Hook
DWORD64 HUDMarkersReturnJMP;
float HUDHeight;
float HUDWidth;
float HUDXOffset;
float HUDYOffset;
float fTwo = (float)2;
void __declspec(naked) HUDMarkers_CC()
{
    __asm
    {
        push rax
        mov eax, [fNewAspect]
        cmp eax, [fNativeAspect]
        jle vertOffset
        jmp horOffset

        vertOffset:
            movd xmm1, [iCustomResX]
            cvtdq2ps xmm1, xmm1
            divss xmm1, [fNativeAspect]
            movss[HUDHeight], xmm1
            movd xmm1, [iCustomResY]
            cvtdq2ps xmm1, xmm1
            subss xmm1, [HUDHeight]
            divss xmm1, [fTwo]
            movss[HUDYOffset], xmm1
            pop rax
            cvtdq2ps xmm0, xmm0                     // Original code
            xorps xmm0, xmm0
            movd xmm1, eax                          // Original code
            cvtdq2ps xmm1, xmm1                     // Original code
            movss xmm1, [HUDYOffset]
            subss xmm3, xmm0                        // Original code
            jmp[HUDMarkersReturnJMP]

        horOffset:
            movd xmm1, [iCustomResY]
            cvtdq2ps xmm1, xmm1
            mulss xmm1, [fNativeAspect]
            movss[HUDWidth], xmm1
            movd xmm1, [iCustomResX]
            cvtdq2ps xmm1, xmm1
            subss xmm1, [HUDWidth]
            divss xmm1, [fTwo]
            movss[HUDXOffset], xmm1
            pop rax  
            cvtdq2ps xmm0, xmm0                     // Original code
            movss xmm0, [HUDXOffset]
            movd xmm1, eax                          // Original code
            cvtdq2ps xmm1, xmm1                     // Original code
            xorps xmm1, xmm1
            subss xmm3, xmm0                        // Original code
            jmp[HUDMarkersReturnJMP]
    }
}

void Logging()
{
    loguru::add_file("StarTrekResurgenceFix.log", loguru::Truncate, loguru::Verbosity_MAX);
    loguru::set_thread_name("Main");

    LOG_F(INFO, "StarTrekResurgenceFix v%s loaded", sFixVer.c_str());
}

void ReadConfig()
{
    // Initialize config
    // UE4 games use launchers so config path is relative to launcher

    std::ifstream iniFile(".\\StarTrekGame\\Binaries\\Win64\\StarTrekResurgenceFix.ini");
    if (!iniFile)
    {
        LOG_F(ERROR, "Failed to load config file.");
        LOG_F(ERROR, "Trying alternate config path.");
        std::ifstream iniFile("StarTrekResurgenceFix.ini");
        if (!iniFile)
        {
            LOG_F(ERROR, "Failed to load config file. (Alternate path)");
            LOG_F(ERROR, "Please ensure that the ini configuration file is in the correct place.");
        }
        else
        {
            ini.parse(iniFile);
            LOG_F(INFO, "Successfuly loaded config file. (Alternate path)");
        }
    }
    else
    {
        ini.parse(iniFile);
        LOG_F(INFO, "Successfuly loaded config file.");
    }

    inipp::get_value(ini.sections["StarTrekResurgenceFix Parameters"], "InjectionDelay", iInjectionDelay);
    inipp::get_value(ini.sections["Fix Aspect Ratio"], "Enabled", bAspectFix);
    iAspectFix = (int)bAspectFix;
    inipp::get_value(ini.sections["Fix FOV"], "Enabled", bFOVFix);
    iFOVFix = (int)bFOVFix;
    inipp::get_value(ini.sections["Fix FOV"], "AdditionalFOV", fAdditionalFOV);
    inipp::get_value(ini.sections["Uncap FPS"], "Enabled", bUncapFPS);
    inipp::get_value(ini.sections["Disable Depth of Field"], "Enabled", bDisableDOF);

    // Custom resolution
    if (iCustomResX > 0 && iCustomResY > 0)
    {
        fNewX = (float)iCustomResX;
        fNewY = (float)iCustomResY;
        fNewAspect = (float)iCustomResX / (float)iCustomResY;
    }
    else
    {
        // Grab desktop resolution
        RECT desktop;
        GetWindowRect(GetDesktopWindow(), &desktop);
        fNewX = (float)desktop.right;
        fNewY = (float)desktop.bottom;
        iCustomResX = (int)desktop.right;
        iCustomResY = (int)desktop.bottom;
        fNewAspect = (float)desktop.right / (float)desktop.bottom;
    }

    // Log config parse
    LOG_F(INFO, "Config Parse: iInjectionDelay: %dms", iInjectionDelay);
    LOG_F(INFO, "Config Parse: bAspectFix: %d", bAspectFix);
    LOG_F(INFO, "Config Parse: bFOVFix: %d", bFOVFix);
    LOG_F(INFO, "Config Parse: fAdditionalFOV: %.2f", fAdditionalFOV);
    LOG_F(INFO, "Config Parse: bUncapFPS: %d", bUncapFPS);
    LOG_F(INFO, "Config Parse: bDisableDOF: %d", bDisableDOF);
    LOG_F(INFO, "Config Parse: fNewX: %.2f", fNewX);
    LOG_F(INFO, "Config Parse: fNewY: %.2f", fNewY);
    LOG_F(INFO, "Config Parse: fNewAspect: %.4f", fNewAspect);
}

void AspectFOVFix()
{

    if (bAspectFix)
    {
        // FSlateRHIRenderer::ConditionalResizeViewport 
        uint8_t* CurrResolutionScanResult = Memory::PatternScan(baseModule, "33 ?? B9 ?? ?? ?? ?? 45 ?? ?? 48 ?? ?? 4A ?? ?? ?? 48 ?? ?? 8B ??");
        if (CurrResolutionScanResult)
        {
            // Grab resolution and aspect ratio
            DWORD64 CurrResolutionAddress = (uintptr_t)CurrResolutionScanResult + 0x7;
            int CurrResolutionHookLength = Memory::GetHookLength((char*)CurrResolutionAddress, 13);
            CurrResolutionReturnJMP = CurrResolutionAddress + CurrResolutionHookLength;
            Memory::DetourFunction64((void*)CurrResolutionAddress, CurrResolution_CC, CurrResolutionHookLength);

            LOG_F(INFO, "Current Resolution: Hook length is %d bytes", CurrResolutionHookLength);
            LOG_F(INFO, "Current Resolution: Hook address is 0x%" PRIxPTR, (uintptr_t)CurrResolutionAddress);
        }
        else if (!CurrResolutionScanResult)
        {
            LOG_F(INFO, "Current Resolution: Pattern scan failed.");
        }

        // UCameraComponent::GetCameraView
        uint8_t* AspectFOVFixScanResult = Memory::PatternScan(baseModule, "F3 0F ?? ?? ?? ?? ?? ?? F3 0F ?? ?? ?? 8B ?? ?? ?? ?? ?? 89 ?? ?? 0F ?? ?? ?? ?? ?? ?? 33 ?? ?? 83 ?? ??");
        if (AspectFOVFixScanResult)
        {
            FOVPiDiv = fPi / 360;
            FOVDivPi = 360 / fPi;

            // Adjust aspect ratio and FOV
            DWORD64 AspectFOVFixAddress = (uintptr_t)AspectFOVFixScanResult + 0x8;
            int AspectFOVFixHookLength = Memory::GetHookLength((char*)AspectFOVFixAddress, 13);
            AspectFOVFixReturnJMP = AspectFOVFixAddress + AspectFOVFixHookLength;
            Memory::DetourFunction64((void*)AspectFOVFixAddress, AspectFOVFix_CC, AspectFOVFixHookLength);

            LOG_F(INFO, "Aspect Ratio/FOV: Hook length is %d bytes", AspectFOVFixHookLength);
            LOG_F(INFO, "Aspect Ratio/FOV: Hook address is 0x%" PRIxPTR, (uintptr_t)AspectFOVFixAddress);
        }
        else if (!AspectFOVFixScanResult)
        {
            LOG_F(INFO, "Aspect Ratio/FOV: Pattern scan failed.");
        }

        // APlayerController::ProjectWorldLocationToScreenWithDistance 
        uint8_t* HUDMarkersScanResult = Memory::PatternScan(baseModule, "0F ?? ?? 66 ?? ?? ?? 0F ?? ?? F3 0F ?? ?? F3 0F ?? ?? F3 0F ?? ?? ?? F3 0F ?? ?? ?? F3 0F ?? ?? 4C");
        if (HUDMarkersScanResult)
        {
            // Offset HUD markers
            DWORD64 HUDMarkersAddress = (uintptr_t)HUDMarkersScanResult;
            int HUDMarkersHookLength = Memory::GetHookLength((char*)HUDMarkersAddress, 13);
            HUDMarkersReturnJMP = HUDMarkersAddress + HUDMarkersHookLength;
            Memory::DetourFunction64((void*)HUDMarkersAddress, HUDMarkers_CC, HUDMarkersHookLength);

            LOG_F(INFO, "HUD Markers: Hook length is %d bytes", HUDMarkersHookLength);
            LOG_F(INFO, "HUD Markers: Hook address is 0x%" PRIxPTR, (uintptr_t)HUDMarkersAddress);
        }
        else if (!HUDMarkersScanResult)
        {
            LOG_F(INFO, "HUD Markers: Pattern scan failed.");
        }
    }

    if (bFOVFix)
    {
        // ULocalPlayer::GetProjectionData 
        uint8_t* FOVCullingScanResult = Memory::PatternScan(baseModule, "8B ?? ?? ?? ?? ?? F2 ?? ?? ?? ?? ?? 89 ?? ?? ?? 84 ?? 75 ??");
        if (FOVCullingScanResult)
        {
            // Fix static mesh culling at high FOV
            DWORD64 FOVCullingAddress = (uintptr_t)FOVCullingScanResult - 0x10;
            int FOVCullingHookLength = Memory::GetHookLength((char*)FOVCullingAddress, 13);
            FOVCullingReturnJMP = FOVCullingAddress + FOVCullingHookLength;
            Memory::DetourFunction64((void*)FOVCullingAddress, FOVCulling_CC, FOVCullingHookLength);

            LOG_F(INFO, "FOV Culling: Hook length is %d bytes", FOVCullingHookLength);
            LOG_F(INFO, "FOV Culling: Hook address is 0x%" PRIxPTR, (uintptr_t)FOVCullingAddress);
        }
        else if (!FOVCullingScanResult)
        {
            LOG_F(INFO, "FOV Culling: Pattern scan failed.");
        }
    }
    else
    {
        fAdditionalFOV = (float)0;
    }
}

void UncapFPS()
{
    if (bUncapFPS)
    {
        // UEngine::GetMaxTickRate
        uint8_t* UncapFPSScanResult = Memory::PatternScan(baseModule, "80 ?? ?? ?? ?? ?? 02 74 ?? F3 0F ?? ?? ?? ?? ?? ?? F3 0F ?? ??");
        if (UncapFPSScanResult)
        {
            // Force bSmoothFrameRate to 0
            DWORD64 UncapFPSAddress = (uintptr_t)UncapFPSScanResult + 0x9;
            int UncapFPSHookLength = Memory::GetHookLength((char*)UncapFPSAddress, 13);
            UncapFPSReturnJMP = UncapFPSAddress + UncapFPSHookLength;
            Memory::DetourFunction64((void*)UncapFPSAddress, UncapFPS_CC, UncapFPSHookLength);

            LOG_F(INFO, "Uncap FPS: Hook length is %d bytes", UncapFPSHookLength);
            LOG_F(INFO, "Uncap FPS: Hook address is 0x%" PRIxPTR, (uintptr_t)UncapFPSAddress);
        }
        else if (!UncapFPSScanResult)
        {
            LOG_F(INFO, "Uncap FPS: Pattern scan failed.");
        }
    } 
}

void GraphicalTweaks()
{
    if (bDisableDOF)
    {
        // DiaphragmDOF::IsSupported 
        uint8_t* DOFScanResult = Memory::PatternScan(baseModule, "74 ?? 41 ?? ?? ?? ?? C1 ?? 08 ?? 01 75 ?? 32 ?? C3 ?? 01 C3");
        if (DOFScanResult)
        {
            // Return value for DOF being enabled is changed to 0
            DWORD64 DOFAddress = (uintptr_t)DOFScanResult + 0x12;
            Memory::PatchBytes((uintptr_t)DOFAddress, "\x00", 1);

            LOG_F(INFO, "Depth of Field: Patch address is 0x%" PRIxPTR, (uintptr_t)DOFAddress);
        }
        else if (!DOFScanResult)
        {
            LOG_F(INFO, "Depth of Field: Pattern scan failed.");
        }
    }
}

DWORD __stdcall Main(void*)
{
    Logging();
    ReadConfig();
    Sleep(iInjectionDelay);
    AspectFOVFix();
    UncapFPS();
    GraphicalTweaks();
    return true; // end thread
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE mainHandle = CreateThread(NULL, 0, Main, 0, NULL, 0);

        if (mainHandle)
        {
            CloseHandle(mainHandle);
        }
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

