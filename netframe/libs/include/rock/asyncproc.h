/**
 *COPYRIGHT NOTICE
 *Copyright (c) 2014,   xunlei.ltd
 *All rights reserved
 *
 *@file asyncproc.h
 *@brief    异步处理任务库
 *
 *@version  1.1
 *@author yangqinghua@xunlei.com
 *@date 2014-01-08
 *
 */

#ifndef _ASYNCPROC_H_
#define _ASYNCPROC_H_

#include <darray.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct async_proc_context * async_proc_t;

    /*
     *@brief 描述任务信息
     *@note callback handle_id is 0,func handle_id is 1
     */
    typedef struct
    {
        union
        {
            int (*callback) (void *arg1, void *arg2, void *arg3);
            int (*func) (darray argv);
        } handle;
        int handle_id;
        void *arg1;
        void *arg2;
        void *arg3;
        darray argv;
    } proc_data_t;

    /*
     *@brief 描述任务错误信息
     *@note 当handle返回-1时产生一个错误
     */
    typedef struct
    {
        int64_t task_id;
        int errcode;
    } proc_err_t;

    /**
     * @brief 初始化任务处理模块
     *
     * @param proc_thread_num :后台处理线程数目
     * @param max_proc_queue_len :处理队列容量
     * @param max_err_queue_len :错误队列容量
     *
     * @return 失败返回NULL，成功返回指向async_proc_context的指针
     */
    async_proc_t init_proc_sys(int proc_thread_num, int max_proc_queue_len, int max_err_queue_len);

    /**
     * @brief 将任务加入处理队列
     *
     * @param proc_context :init_proc_sys 返回的任务处理模块描述信息
     * @param data :任务信息指针
     *
     * @return 失败返回-1，成功加入后端队列返回taskid
     * @note 函数成功返回，仅表示成功加入处理队列
     */
    int64_t proc(async_proc_t proc_context, proc_data_t * data);

    /**
     * @brief 获取任务列表执行出现的一个错误
     *
     * @param proc_context :init_proc_sys 返回的任务处理模块描述信息
     *
     * @return proc_context 中的一个错误
     */
    proc_err_t get_proc_err(async_proc_t proc_context);

    /**
     * @brief 获取当前任务处理队列的大小
     *
     * @param proc_context :init_proc_sys 返回的任务处理模块描述信息
     *
     * @return proc_context中当前任务队列的大小
     */
    int get_current_proc_queue_size(async_proc_t proc_context);

    /**
     * @brief 销毁proc_context
     *
     * @param proc_context :init_proc_sys 返回的任务处理模块描述信息
     */
    void destroy_proc_sys(async_proc_t proc_context);

#ifdef __cplusplus
}
#endif
#endif
