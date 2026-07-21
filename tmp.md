```C
static void pwm_handle_in_breath_anim(void)
{
    static volatile u8 breath_step_cnt = 0;
    // 新增：统一的渐变进度，0~100 代表 0%~100%
    static volatile u8 breath_progress = 0; 

    switch (pwm_handle_param.breath_anim_sta)
    {
    case PWM_BREATH_ANIM_STA_INIT:
        breath_step_cnt = 0;
        breath_progress = 0; // 初始化进度为0

        if (pwm_handle_param.cur_pwm_0_duty_val || pwm_handle_param.cur_pwm_1_duty_val)
        {
            // 当前有亮度，说明是从中间打断进入的，计算当前进度
            // 防止除0，需确保dest不为0
            if (pwm_handle_param.dest_pwm_0_duty_val > 0) {
                breath_progress = (u32)pwm_handle_param.cur_pwm_0_duty_val * 100 / pwm_handle_param.dest_pwm_0_duty_val;
            }
            pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_DOWN;
        }
        else
        {
            pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_UP;
        }
        break;

    case PWM_BREATH_ANIM_STA_UP:
        // 统一步调：进度 +1
        if (breath_progress < 100) {
            breath_progress++;
        }

        // 根据统一进度，同步计算两路 PWM 的当前占空比
        pwm_handle_param.cur_pwm_0_duty_val = (u32)pwm_handle_param.dest_pwm_0_duty_val * breath_progress / 100;
        pwm_handle_param.cur_pwm_1_duty_val = (u32)pwm_handle_param.dest_pwm_1_duty_val * breath_progress / 100;

        if (breath_progress >= 100)
        {
            // 渐亮完成，切换到渐灭
            pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_DOWN;
            breath_step_cnt++;
        }
        break;

    case PWM_BREATH_ANIM_STA_DOWN:
        // 统一步调：进度 -1
        if (breath_progress > 0) {
            breath_progress--;
        }

        // 根据统一进度，同步计算两路 PWM 的当前占空比
        pwm_handle_param.cur_pwm_0_duty_val = (u32)pwm_handle_param.dest_pwm_0_duty_val * breath_progress / 100;
        pwm_handle_param.cur_pwm_1_duty_val = (u32)pwm_handle_param.dest_pwm_1_duty_val * breath_progress / 100;

        if (breath_progress == 0)
        {
            // 渐灭完成，切换到渐亮
            pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_UP;
            breath_step_cnt++;
        }
        break;
    }

    if (breath_step_cnt >= (5 * 2))
    {
        breath_step_cnt = 0;
        breath_progress = 0;
        pwm_handle_param.cur_mode = pwm_handle_param.last_mode;
    }

    // 同步更新硬件输出
    set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
    set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);
}
```