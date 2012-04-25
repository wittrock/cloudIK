ip=`wget -qO - icanhazip.com`
ip="http://$ip/cgi-bin/cloudIKCGI.cgi?link=r_wrist_roll_link&x=0.75&y=-0.188&z=0&roll=0&pitch=0&yaw=0"
echo $ip