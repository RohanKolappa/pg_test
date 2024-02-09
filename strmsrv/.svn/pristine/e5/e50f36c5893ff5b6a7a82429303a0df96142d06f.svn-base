/*
 * cpustats.hh
 *
 *  Created on: Jul 4, 2010
 *      Author: rkale
 */

#ifndef CPUSTATS_HH_
#define CPUSTATS_HH_

#define CPUWIN          32

class CCommand;

class CCpuStats {
public:
    CCpuStats();
    ~CCpuStats();

    void ComputeCpuUsage();
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    FILE * m_procfp;
    double m_cpuuser;
    double m_cpusystem;
    double m_cpuidle;
    double m_cpuiowait;
    double m_cpuirq;
    double m_cpusoftirq;
    double m_cpuLoad;
    double m_cpuAvgLoad;
    double m_cpuloadarr[CPUWIN];
    int m_cpuloadidx;
    uint64_t m_userval;
    uint64_t m_systemval;
    uint64_t m_niceval;
    uint64_t m_idleval;
    uint64_t m_iowaitval;
    uint64_t m_irqval;
    uint64_t m_softirqval;
    uint64_t m_totalval;
};

#endif /* CPUSTATS_HH_ */
