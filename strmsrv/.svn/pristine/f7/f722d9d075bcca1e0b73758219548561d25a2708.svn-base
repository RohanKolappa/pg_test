/*
 * cpustats.cpp
 *
 *  Created on: Jul 4, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include "cpustats.hh"
#include "command.hh"

CCpuStats::CCpuStats()
{
    m_userval = 0;
    m_systemval = 0;
    m_niceval = 0;
    m_idleval = 0;
    m_irqval = 0;
    m_iowaitval = 0;
    m_softirqval = 0;
    m_totalval = 0;


    m_cpuuser = 0.0;
    m_cpusystem = 0.0;
    m_cpuidle = 0.0;
    m_cpuLoad = 0.0;
    m_cpuAvgLoad = 0.0;
    m_cpuloadidx = 0;
    m_cpuiowait = 0.0;
    m_cpusoftirq = 0;
    m_cpuirq = 0;

    for (int i = 0; i < CPUWIN; i++)
        m_cpuloadarr[i] = 0.0;

    m_procfp = fopen("/proc/stat", "r");

}

CCpuStats::~CCpuStats()
{
    if (m_procfp != NULL)
        fclose(m_procfp);
}

void CCpuStats::ComputeCpuUsage()
{

    uint64_t userval, systemval, niceval, idleval;
    uint64_t iowaitval, irqval, softirqval, stealval;

    // enough for a /proc/stat CPU line (not the intr line)
    char lbuf[256];

    if (m_procfp == NULL)
        return;

    /* Rewind to read first line again */
    rewind(m_procfp);
    fflush(m_procfp);

    // first line is the cpu summary line
    if (!fgets(lbuf, sizeof(lbuf), m_procfp))
        return;

    if (4 > sscanf(lbuf, "cpu %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" "
            "%"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64"",
                   &userval, &niceval, &systemval, &idleval,
                   &iowaitval, &irqval, &softirqval, &stealval))
        return;

    uint64_t userval_d = userval - m_userval;
    uint64_t systemval_d = systemval - m_systemval;
    uint64_t idleval_d = idleval - m_idleval;
    uint64_t iowaitval_d = irqval - m_irqval;
    uint64_t irqval_d = irqval - m_irqval;
    uint64_t softirqval_d = softirqval - m_softirqval;

    uint64_t totalval = userval + systemval + niceval + idleval +
               + iowaitval + irqval + softirqval + stealval;

    uint64_t totalval_d = totalval - m_totalval;
    if (totalval_d < 1)
        totalval_d = 1;

    m_cpuuser = (100.0 * (double) userval_d) / (double) totalval_d;
    m_cpusystem = (100.0 * (double) systemval_d) / (double) totalval_d;
    m_cpuidle = (100.0 * (double) idleval_d) / (double) totalval_d;
    m_cpuiowait = (100.0 * (double) iowaitval_d) / (double) totalval_d;
    m_cpuirq = (100.0 * (double) irqval_d) / (double) totalval_d;
    m_cpusoftirq = (100.0 * (double) softirqval_d) / (double) totalval_d;

    double load = 100.0 - (m_cpuidle + m_cpuiowait);
    m_cpuLoad -= m_cpuloadarr[m_cpuloadidx];
    m_cpuloadarr[m_cpuloadidx++] = load;
    m_cpuloadidx = m_cpuloadidx % CPUWIN;
    m_cpuLoad += load;
    m_cpuAvgLoad = m_cpuLoad / (double)CPUWIN;

    m_userval = userval;
    m_systemval = systemval;
    m_niceval = niceval;
    m_idleval = idleval;
    m_iowaitval = iowaitval;
    m_irqval = irqval;
    m_softirqval = softirqval;
    m_totalval = totalval;
}

void CCpuStats::UpdateCustomStatus(CCommand *pCmd)
{
    ComputeCpuUsage();

    pCmd->SetObjectParam("User", (double) m_cpuuser);
    pCmd->SetObjectParam("System", (double) m_cpusystem);
    pCmd->SetObjectParam("Interrupt", (double) m_cpuirq);
    pCmd->SetObjectParam("SoftInterrupt", (double) m_cpusoftirq);
    pCmd->SetObjectParam("Idle", (double) m_cpuidle);
    pCmd->SetObjectParam("Load", (double) (100.0 - m_cpuidle));
    pCmd->SetObjectParam("AvgLoad", (double) m_cpuAvgLoad);
}
