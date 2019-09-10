import cv2
import numpy as np
import time
import math
import serial
from collections import deque
import random

# To keep track of all point where object visited
center_points = deque()

# I have refer to the code from csdn, the website is：
# https://blog.csdn.net/sinat_31135199/article/details/51252393

# set threshold value for red color in HSV
redLower = np.array([170, 100, 100])
redUpper = np.array([179, 255, 255])

# initialize tracing point
mybuffer = 64
pts = deque(maxlen=mybuffer)

# initialize horizontal and vertical wall matrix
hwall_matrix = np.zeros((6, 9), dtype=np.uint64)
vwall_matrix = np.zeros((5, 10), dtype=np.uint64)
hwall_matrix[0, :] = 1
hwall_matrix[-1, :] = 1
vwall_matrix[:, 0] = 1
vwall_matrix[:, -1] = 1

# print('dimension', hwall_shape[2])
print('hwall is: ', hwall_matrix)
print('vwall is: ', vwall_matrix)
# define the original point
ORIG_X = 10
ORIG_Y = 18

# define horizontal and vertical thread value
HORIZ_L = 90
VERTI_L = 80

# define the x coordinates of vertical wall mid points
vwall_MidX = [3, 89, 179, 269, 354, 439, 526, 604, 694, 779]
vwall_MidY = [50, 138, 226, 314, 402]
hwall_MidX = [48, 138, 218, 308, 398, 478, 563, 653, 743]
hwall_MidY = [18, 98, 188, 268, 358, 438]

vxMod = []
global start_cell_row
global start_cell_col
global heading

ard = ""


def track_red(image_frame):
    # frame = cv2.flip(image_frame, 1)
    frame = image_frame
    # Blur the frame a little
    blur_frame = cv2.GaussianBlur(frame, (7, 7), 0)

    # Convert from BGR to HSV color format
    hsv = cv2.cvtColor(blur_frame, cv2.COLOR_BGR2HSV)

    # Define lower and upper range of hsv color to detect. Blue here
    # lower_blue = np.array([100, 50, 50])
    # upper_blue = np.array([140, 255, 255])

    # red from Catherine
    redLower = np.array([170, 100, 100])
    redUpper = np.array([179, 255, 255])

    # red from Blair
    # redLower = np.array([145, 0, 0])
    # redUpper = np.array([255, 102, 215])
    mask = cv2.inRange(hsv, redLower, redUpper)

    # Make elliptical kernel
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (15, 15))

    # Opening morph(erosion followed by dilation)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)

    # Find all contours
    contours, hierarchy = cv2.findContours(mask.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)[-2:]

    if len(contours) > 0:
        # Find the biggest contour
        biggest_contour = max(contours, key=cv2.contourArea)

        # Find center of contour and draw filled circle
        moments = cv2.moments(biggest_contour)
        centre_of_contour = (int(moments['m10'] / moments['m00']), int(moments['m01'] / moments['m00']))
        cv2.circle(frame, centre_of_contour, 5, (0, 0, 255), -1)

        # Bound the contour with circle
        ellipse = cv2.fitEllipse(biggest_contour)
        cv2.ellipse(frame, ellipse, (0, 255, 255), 2)

        # Save the center of contour so we draw line tracking it
        center_points.appendleft(centre_of_contour)

    # Draw line from center points of contour
    for i in range(1, len(center_points)):
        b = random.randint(230, 255)
        g = random.randint(100, 255)
        r = random.randint(100, 255)
        if math.sqrt(((center_points[i - 1][0] - center_points[i][0]) ** 2) + (
                (center_points[i - 1][1] - center_points[i][1]) ** 2)) <= 50:
            cv2.line(frame, center_points[i - 1], center_points[i], (b, g, r), 4)

    cv2.imshow('original', frame)
    cv2.imshow('mask', mask)

    return

# def send_data_to_arduino_testing_lee(serial):
#     #################Things need to be updated########################
#     port = 'COM4'  # windows
#     # port = 'COM7'
#     ##################################################################
#     # ard = serial.Serial(port, 115200, timeout=5)
#     ard = serial.Serial(port, 115200, timeout=5)
#     time.sleep(3)  # wait for Arduino
#
#     return ard


# def send_data_to_arduino_testing_vbc(serial, horiz_wall, verti_wall, carx, cary, heading, ard):
#     vision_flag = "v"
#     vision_flag_str = str.encode(vision_flag)
#     ard.write(vision_flag_str)
#
#     msg = ard.read(ard.inWaiting())  # read all characters in buffer
#     print("Message from arduino: ")
#     print(msg)
#     time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
#
#     vision_flag = "b"
#     vision_flag_str = str.encode(vision_flag)
#     print("b send")
#     print(vision_flag_str)
#     ard.write(vision_flag_str)
#
#     msg = ard.read(ard.inWaiting())  # read all characters in buffer
#     print("Message from arduino: ")
#     print(msg)
#     time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
#
#     vision_flag = "c"
#     vision_flag_str = str.encode(vision_flag)
#     print("c send")
#     print(vision_flag_str)
#     ard.write(vision_flag_str)
#
#     msg = ard.read(ard.inWaiting())  # read all characters in buffer
#     print("Message from arduino: ")
#     print(msg)
#     time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
#     return

def send_data_to_arduino(serial, horiz_wall, verti_wall, carx, cary, heading):

    # #################Things need to be updated########################
    port = 'COM4'  # windows
    # # port = 'COM7'
    # ##################################################################
    # ard = serial.Serial(port, 115200, timeout=5)
    ard = serial.Serial(port, 115200, timeout=5)
    time.sleep(3)  # wait for Arduino

    i = 0
    vision_flag = "v"
    vision_flag_str = str.encode(vision_flag)
    ard.write(vision_flag_str)

    msg = ard.read(ard.inWaiting())  # read all characters in buffer
    print("Message from arduino: ")
    print(msg)
    time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code

    while i < 1:
        # Serial write section
        ard.flush()
        # ard.flushInput()
        print("Python value sent: ")
        # print('mtrn4110')
        cell_string = ''

        rr = 0
        rc = 0

        while rr < 5:
            r_str = str(rr)

            while rc < 9:
                c_str = str(rc)
                # define the wall config:
                # up, right, down, left
                left = verti_wall[rr][rc]
                right = verti_wall[rr][rc + 1]
                up = horiz_wall[rr][rc]
                down = horiz_wall[rr + 1][rc]

                up_s = np.left_shift(int(up), 3)
                right_s = np.left_shift(int(right), 2)
                down_s = np.left_shift(int(down), 1)
                left_s = int(left)

                # treat the wall status as an integer
                wall = up_s + right_s + down_s + left_s
                # convert this integer to hex
                wall_str = hex(wall).split('0x')[-1].upper()
                # print('wall str: ', wall_str)
                cell_pos = r_str + c_str + wall_str
                cell_string = cell_string + cell_pos

                rc = rc + 1
                # print('rc is: ', rc)
            cell_pos = ''
            rc = 0
            rr = rr + 1
            # print('rr is: ', rr)


        # vision_flag = "b"
        # vision_flag_str = str.encode(vision_flag)
        # print("b send")
        # print(vision_flag_str)
        # ard.write(vision_flag_str)
        #
        # msg = ard.read(ard.inWaiting())  # read all characters in buffer
        # print("Message from arduino: ")
        # print(msg)
        # time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
        #
        # vision_flag = "c"
        # vision_flag_str = str.encode(vision_flag)
        # print("c send")
        # print(vision_flag_str)
        # ard.write(vision_flag_str)
        #
        # msg = ard.read(ard.inWaiting())  # read all characters in buffer
        # print("Message from arduino: ")
        # print(msg)
        # time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
        # send the string that consists of map
        # split the string into three parts
        # send the layout for the first two rows first
        first_part_map = cell_string[:54]
        first_part_cell_string_to_send = "b" + first_part_map
        maze_layout_first_part = str.encode(first_part_cell_string_to_send)
        print(maze_layout_first_part)
        ard.write(maze_layout_first_part)
        time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code

        # Serial read section
        msg = ard.read(ard.inWaiting())  # read all characters in buffer
        print("Message from arduino: ")
        print(msg)
        time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code

        # while msg != first_part_cell_string_to_send:
        #     ard.write(maze_layout_first_part)
        #     msg = ard.read(ard.inWaiting())
        #     print("Message from arduino in loop: ")
        #     print(msg)
        #     if msg == first_part_cell_string_to_send:
        #         break

        # send the data of the third and the fourth row
        second_part_map = cell_string[54:108]
        second_part_cell_string_to_send = "b" + second_part_map
        maze_layout_second_part = str.encode(second_part_cell_string_to_send)
        print(maze_layout_second_part)
        ard.write(maze_layout_second_part)
        time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code

        # Serial read section
        msg = ard.read(ard.inWaiting())  # read all characters in buffer
        print("Message from arduino: ")
        print(msg)

        # while msg != second_part_cell_string_to_send:
        #     ard.write(maze_layout_second_part)
        #     msg = ard.read(ard.inWaiting())
        #     print("Message from arduino in loop: ")
        #     print(msg)
        #     if msg == second_part_cell_string_to_send:
        #         break

        third_part_map = cell_string[108:]
        third_part_cell_string_to_send = "b" + third_part_map
        maze_layout_third_part = str.encode(third_part_cell_string_to_send)
        print(maze_layout_third_part)
        ard.write(maze_layout_third_part)
        time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code

        # Serial read section
        msg = ard.read(ard.inWaiting())  # read all characters in buffer
        print("Message from arduino: ")
        print(msg)

        # while msg != third_part_cell_string_to_send:
        #     ard.write(maze_layout_third_part)
        #     msg = ard.read(ard.inWaiting())
        #     print("Message from arduino in loop: ")
        #     print(msg)
        #     if msg == third_part_cell_string_to_send:
        #         break

        i = i + 1
        print("-----------------------------")
    else:
        print("Exiting")

    time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
    # send the car pos (start point), heading , and the goal position through the Bluetooth
    converted_heading = ""
    if heading == "N":
        converted_heading = "S"

    elif heading == "S":
        converted_heading = "N"

    elif heading == "W":
        converted_heading = "E"

    elif heading == "E":
        converted_heading = "W"

    car_pos_and_goal_pos = "c" + str(int(carx)) + str(int(cary)) + converted_heading + "2" + "4"
    print("car pos is: ", car_pos_and_goal_pos)
    car_pos = str.encode(car_pos_and_goal_pos)
    print(car_pos)
    ard.write(car_pos)

    # Serial read section
    msg = ard.read(ard.inWaiting())  # read all characters in buffer
    print("Message from arduino: ")
    print(msg)

    # while msg != car_pos_and_goal_pos:
    #     ard.write(car_pos_and_goal_pos)
    #     msg = ard.read(ard.inWaiting())
    #     print("Message from arduino in loop: ")
    #     print(msg)
    #     if msg == car_pos_and_goal_pos:
    #         break

    time.sleep(3)  # shortened or lengthen this to match the new value in your Arduino code
    start_signal = "12"
    print("start signal is: ", start_signal)
    start_signal = str.encode(start_signal)
    print(start_signal)
    ard.write(start_signal)

    # Serial read section
    msg = ard.read(ard.inWaiting())  # read all characters in buffer
    print("Message from arduino: ")
    print(msg)

    # while msg != start_signal:
    #     ard.write(start_signal)
    #     msg = ard.read(ard.inWaiting())
    #     print("Message from arduino in loop: ")
    #     print(msg)
    #     if msg == start_signal:
    #         break
    ard.close()
    return


def get_heading(redx, redy, bluex, bluey):

    dir = 'NSWE'

    # if abs(bluey - redy) <= 20 and redx > bluex:
    #     return dir[0]
    # elif abs(bluey - redy) <= 20 and redx < bluex:
    #     return dir[1]
    # elif abs(bluex - redx) <= 20 and redy < bluey:
    #     return dir[2]
    # elif abs(bluex - redx) <= 20 and redy > bluey:
    #     return dir[3]

    if redx > bluex and abs(redy - bluey) < abs(redx - bluex):
        return dir[0]
    elif redx < bluex and abs(redy - bluey) < abs(redx - bluex):
        return dir[1]
    elif redy < bluey and abs(redy - bluey) > abs(redx - bluex):
        return dir[2]
    elif redy > bluey and abs(redy - bluey) > abs(redx - bluex):
        return dir[3]


def find_and_draw_blue_circle_on_robot(original_image, cv2):
    bx = 0
    by = 0
    # Step1. convert to HSV color space and read image
    image_hue = cv2.cvtColor(original_image, cv2.COLOR_BGR2HSV)
    # walls_obtained = cv2.imread("draw_processed_walls1.jpg")
    walls_obtained = cv2.imread("processed_image.jpg")

    # Step2. use color for segmentation
    # low range for low threshold
    # high range for high threshold

    # blue_low_bound = np.array([110, 50, 50], np.uint8)
    # blue_high_bound = np.array([130, 255, 255], np.uint8)
    blue_low_bound = np.array([100, 50, 50], np.uint8)
    blue_high_bound = np.array([255, 160, 160], np.uint8)
    th = cv2.inRange(image_hue, blue_low_bound, blue_high_bound)

    # Step3. do a dilation
    dilated_img = cv2.dilate(th, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3)), iterations=2)

    # Step4. Use Hough to find Circle
    blue_circles = cv2.HoughCircles(dilated_img, cv2.HOUGH_GRADIENT, 1, 100, param1=15, param2=7, minRadius=10,
                                    maxRadius=20)
    # blue_circles = cv2.HoughCircles(dilated_img, cv2.HOUGH_GRADIENT, 1, 50, param1=15, param2=7, minRadius=10,
    #                                 maxRadius=20)

    # Step5. draw the circle
    if blue_circles is not None:
        print("blue circles")
        bx, by, bradius = blue_circles[0][0]
        blue_center = (bx, by)
        cv2.circle(walls_obtained, blue_center, bradius, (255, 255, 255), 2)
        # cv2.imwrite("draw_processed_walls1.jpg", walls_obtained)
        cv2.imwrite("processed_image.jpg", walls_obtained)
    return bx, by;


def find_and_draw_red_circle_on_robot(original_image, cv2):
    rx = 0
    ry = 0
    # Step1. convert to HSV color space and read image
    image_hue = cv2.cvtColor(original_image, cv2.COLOR_BGR2HSV)
    walls_obtained = cv2.imread("processed_image.jpg")

    # Step2. use color for segmentation
    # low range for low threshold
    # high range for high threshold
    #
    # red_low_bound = np.array([0, 123, 100])
    # red_high_bound = np.array([5, 255, 255])
    # red_low_bound = np.array([170, 100, 50])

    # from Catherine
    red_low_bound = np.array([170, 100, 100])
    red_high_bound = np.array([179, 255, 255])

    # from Blair
    # red_low_bound = np.array([145, 0, 0])
    # red_high_bound = np.array([255, 102, 215])
    # red_low_bound = np.array([20, 100, 100])
    # red_high_bound = np.array([220, 255, 255])

    #
    # red_low_bound = np.array([157, 29, 25])
    # red_high_bound = np.array([185, 50, 51])

    # try this11111111
    # red_low_bound = np.array([157, 100, 100])
    # red_high_bound = np.array([185, 255, 255])

    # red_high_bound = np.array([20, 255, 255])

    th = cv2.inRange(image_hue, red_low_bound, red_high_bound)

    # Step3. do a dilation
    dilated_img = cv2.dilate(th, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3)), iterations=2)

    # Step4. Use Hough to find Circle
    red_circles = cv2.HoughCircles(dilated_img, cv2.HOUGH_GRADIENT, 1, 100, param1=15, param2=7, minRadius=10,
                                   maxRadius=20)

    # red_circles = cv2.HoughCircles(dilated_img, cv2.HOUGH_GRADIENT, 1, 50, param1=15, param2=7, minRadius=10,
    #                               maxRadius=20)

    # red_circles = cv2.HoughCircles(dilated_img, cv2.HOUGH_GRADIENT, 1, 100, param1=15, param2=7, minRadius=1,
    #                               maxRadius=40)

    # Step5. draw the circle
    if red_circles is not None:
        print("red circles")
        rx, ry, rradius = red_circles[0][0]
        red_center = (rx, ry)
        cv2.circle(walls_obtained, red_center, rradius, (255, 255, 255), 2)
        # cv2.imwrite("draw_processed_walls1.jpg", walls_obtained)
        cv2.imwrite("processed_image.jpg", walls_obtained)
    return rx, ry;


def draw_horizontal_walls(hwall_midx, hwall_midy, hwall_mat, hlength, image1, cv2):
    hwall_shape = np.shape(hwall_matrix)
    rows = hwall_shape[0]
    cols = hwall_shape[1]

    for r in range(rows):
        for c in range(cols):
            # if there is a wall
            if hwall_mat[r, c] == 1:
                startx = hwall_midx[c]
                starty = hwall_midy[r]
                cv2.line(image1, (startx-hlength//2, starty), (startx + hlength//2, starty), (0, 0, 255), 2)
                # cv2.imwrite("draw_processed_walls1.jpg", image1)
                cv2.imwrite("processed_image.jpg", image1)
            else:
                continue
    return


def draw_vertical_walls(vwall_midx, vwall_midy, vwall_mat, vlength, image1, cv2):
    vwall_shape = np.shape(vwall_matrix)
    rows = vwall_shape[0]
    cols = vwall_shape[1]

    for r in range(rows):
        for c in range(cols):
            # if there is a wall
            if vwall_mat[r, c] == 1:
                startx = vwall_midx[c]
                starty = vwall_midy[r]
                cv2.line(image1, (startx, starty - vlength // 2), (startx, starty + vlength // 2), (0, 0, 255), 2)
                # cv2.imwrite("draw_processed_walls1.jpg", image1)
                cv2.imwrite("processed_image.jpg", image1)
            else:
                continue

    return


def track_horizontal_walls(p1x, p1y, p2x, p2y, vdist, orig_y, hwall_midx_arr, hwall_midy_arr, hwall_mat):

    x1 = p1x
    y1 = p1y
    x2 = p2x
    y2 = p2y
    # vyMod = []
    y1v = abs(y1 - orig_y) // vdist
    y1vMod = abs(y1 - orig_y) % vdist
    # print('x1 mod is: ', y1vMod)
    # vyMod.append(y1vMod)

    if y1vMod >= VERTI_L / 2:
        hrow = y1v + 1
    else:
        hrow = y1v

    h_midy = hwall_midy_arr[hrow]

    j = 0
    while j < len(hwall_midx_arr):
        h_midx = hwall_midx_arr[j]
        # calculate the midpoint of detected line
        line_midx = (x1 + x2) / 2
        line_midy = (y1 + y2) / 2
        hdiff = line_midx - h_midx
        vdiff = line_midy - h_midy
        distance = math.sqrt(hdiff * hdiff + vdiff * vdiff)

        # if the distance is within 25 pixels
        if distance <= 25:
            hcol = j
            hwall_mat[hrow, hcol] = 1
            break

        j += 1
    return hwall_mat


def track_vertical_walls(p1x, p1y, p2x, p2y, hdist, orig_x, vwall_midx_arr, vwall_midy_arr, vwall_mat):

    x1 = p1x
    y1 = p1y
    x2 = p2x
    y2 = p2y
    # vxMod = []
    x1h = abs(x1 - orig_x) // hdist
    x1hMod = abs(x1 - orig_x) % hdist

    # vxMod.append(x1hMod)

    if x1hMod >= HORIZ_L / 2:
        vcol = x1h + 1
    else:
        vcol = x1h

    v_midx = vwall_midx_arr[vcol]

    j = 0
    while j < len(vwall_midy_arr):
        v_midy = vwall_midy_arr[j]
        # calculate the midpoint of detected line
        line_midx = (x1 + x2) / 2
        line_midy = (y1 + y2) / 2
        hdiff = line_midx - v_midx
        vdiff = line_midy - v_midy
        distance = math.sqrt(hdiff * hdiff + vdiff * vdiff)

        # if the distance is within 25 pixels
        if distance <= 25:
            vrow = j
            vwall_mat[vrow, vcol] = 1
            break

        j += 1

    return vwall_matrix


# img is the color image
# a , b are the parameters used to adjust intensity
def adjust_intensity(image, a, b):

    rows, cols, channels = img.shape
    dst = image

    for i in range(rows):
        for j in range(cols):
            for c in range(3):
                color = img[i, j][c] * a + b
                if color > 255:  # ensure value between （0~255）
                    dst[i, j][c] = 255
                elif color < 0:  # ensure value between （0~255）
                    dst[i, j][c] = 0
    return dst


def img_dilate(original):
    # use a rectangle kernel
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
    dilation = cv2.dilate(original, kernel, iterations=1)
    return dilation


def img_erode(original):
    # use a rectangle kernel
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
    erosion = cv2.erode(original, kernel, iterations=1)
    return erosion


def img_store(image):
    # store a processed image
    img_name = "my_frame_image.png"
    cv2.imwrite(img_name, image)
    print("image stored")
    return


def track_object():

    return

# open camera
camera = cv2.VideoCapture(cv2.CAP_DSHOW+1)
# camera = cv2.VideoCapture(0)

# camera = cv2.VideoCapture(0)
camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1000)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 800)
camera.set(cv2.CAP_PROP_FPS, 30)
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
# try 640 800
out = cv2.VideoWriter('output.mp4', fourcc, 30.0, (1000, 800))

# wait for two seconds
time.sleep(2)
# check red frame

itr = 0

while True:

    # # read a frame
    (ret, frame) = camera.read()
    #
    # if open successful
    if not ret:
        print('No Camera')
    #
    # Display the resulting frame
    cv2.imshow('frame', frame)
    k = cv2.waitKey(1)

    if k & 0xFF == ord('t'):
      cv2.imwrite("capture.jpg", frame)
    if k & 0xFF == ord('c'):

        # fo testing bluetooth
        # cv2.imwrite("first frame.jpg", frame)

        # for final testing
        cv2.imwrite("first frame original.jpg", frame)
        # cv2.imwrite("first frame final.jpg", frame)

        img = cv2.imread("first frame original.jpg")
        # img = cv2.imread('testing_tom_6.jpg')
        cv2.imwrite("processed_image.jpg", img)
        # img = cv2.imread('first frame.jpg')
        # cv2.imwrite("draw_processed_walls1.jpg", frame)

        # blank_walls_img = cv2.imread('draw_processed_walls1.jpg')
        # cv2.imshow("blank walls", blank_walls_img)
        height, width = img.shape[:2]
        print('height is: ', height)
        print('width is： ', width)
        # img_resize = cv2.resize(frame, (800, 448))
        img_resize = cv2.resize(img, (800, 448))
        img = cv2.imwrite("resize image.jpg", img_resize)
        img = cv2.imread("resize image.jpg")
        img_copy = img.copy()
        img_copy1 = img.copy()
        img_copy2 = img.copy()
        img_copy3 = img.copy()
        img_copy4 = img.copy()
        # convert to gray image
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        # use gaussian blur
       #  gray = cv2.GaussianBlur(gray, (5, 5), 0)
        # get the image shape
        rows, cols, channels = img.shape
        gray_copy = gray.copy()
        (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray_copy)
        print("max val is: ", maxVal)
        print("min val is: ", minVal)
        for row in range(rows):
            for col in range(cols):
                 if gray_copy[row][col] >= maxVal - 20: # morning: 190, evening: 230
                    gray_copy[row][col] = 150  # morning: 150, evening: 170

        # cv2.imshow('modified', gray_copy)
        cv2.imwrite('gray1000.jpg', gray_copy)
        # morning 80, 160
        # evening 60, 170
        gray_copy = gray.copy()
        #try 200
        # try 100, 200
        # used to be 60 170
        edge_filtered0 = cv2.Canny(gray_copy, 60, 170, apertureSize=3)

        cv2.imshow('Pure Canny', edge_filtered0)

        # image = img.copy()
        # cv2.circle(image, maxLoc, 20, (255, 0, 0), 2)
        # cv2.imshow("Robust", image)

        # lines = cv2.HoughLinesP(edge_filtered0, 1, np.pi / 180, 20, 20, 20)
        # lines = cv2.HoughLinesP(edge_filtered0, 1, np.pi / 180, 20, 20, 20)
        lines = cv2.HoughLinesP(edge_filtered0, 1, np.pi / 180, 20, 10, 20)
        i = 0

        # used to set the vertical wall array for future planning
        vrow = 0
        vcol = 0

        # used to store mid points coordinates for association
        v_midx = 0
        v_midy = 0

        print('There are: ', len(lines))
        while i < len(lines):
            for x1, y1, x2, y2 in lines[i]:
                cv2.line(img_copy, (x1, y1), (x2, y2), (0, 0, 255), 2)
                cv2.imwrite('pure Hough.jpg', img_copy)

                # get rid of the slope lines
                if abs(x2 - x1) > 5 and abs(y1 - y2) > 5:
                    continue

                # track vertical lines
                if x1 == x2 or abs(x1 - x2) <= 10:

                    track_vertical_walls(x1, y1, x2, y2, HORIZ_L, ORIG_X, vwall_MidX, vwall_MidY,
                                         vwall_matrix)
                    # draw_vertical_walls(vwall_MidX, vwall_MidY, vwall_matrix, VERTI_L, img_copy1, cv2)

                # track horizontal lines
                elif y1 == y2 or abs(y1 - y2) <= 10:

                    track_horizontal_walls(x1, y1, x2, y2, VERTI_L, ORIG_Y, hwall_MidX, hwall_MidY,
                                           hwall_matrix)

                    # draw_horizontal_walls(hwall_MidX, hwall_MidY, hwall_matrix, HORIZ_L, img_copy1, cv2)

            print("i is:", i)
            i = i + 1

        # greenx, greeny = find_and_draw_green_circle_on_robot(img, cv2)
        # img = cv2.imread("processed_image.jpg")

        draw_vertical_walls(vwall_MidX, vwall_MidY, vwall_matrix, VERTI_L, img_copy1, cv2)
        draw_horizontal_walls(hwall_MidX, hwall_MidY, hwall_matrix, HORIZ_L, img_copy1, cv2)

        font = cv2.FONT_HERSHEY_SIMPLEX
        processed_img = cv2.imread("processed_image.jpg")
        redx, redy = find_and_draw_red_circle_on_robot(img, cv2)
        bluex, bluey = find_and_draw_blue_circle_on_robot(img, cv2)
        print('red x is: ', redx)
        print('red y is: ', redy)
        print('blue x is: ', bluex)
        print('blue y is: ', bluey)
        heading = get_heading(redx, redy, bluex, bluey)
        print("heading is: ", heading)
        cv2.putText(processed_img, heading, (100, 100), font, 4, (255, 255, 255), 2, cv2.LINE_AA)
        cv2.imshow("TEXT PUT", processed_img)

        car_midx = (redx + bluex)/2
        car_midy = (redy + bluey)/2
        print("car_midx is: ")
        print("car_midy is: ")
        xdiff = abs(redx - bluex)
        ydiff = abs(redy - bluey)
        car_posx = car_midx + xdiff/3
        car_posy = car_midy + ydiff/3

        #try this
        print("car position x: ", bluex)
        print("car position y: ", bluey)
        # start_cell_row = (bluey - ORIG_Y) // VERTI_L
        # start_cell_col = (bluex - ORIG_X) // HORIZ_L
        start_cell_row = car_posy // VERTI_L
        start_cell_col = car_posx // HORIZ_L

        if start_cell_row > 4:
            start_cell_row = 4

        if start_cell_col > 8:
            start_cell_col = 8

        print("car starts at row: ", start_cell_row)
        print("car starts at col: ", start_cell_col)
        # send_data_to_arduino(serial, hwall_matrix, vwall_matrix, start_cell_row, start_cell_col, heading)
        print("hwall matrix", hwall_matrix)
        print("vall_matrix", vwall_matrix)

    # if press q, quit
    if k & 0xFF == ord('q'):
        break

    if k & 0xFF == ord('s'):
        start_cell_row = 4
        start_cell_col = 8
        heading = "W"
        send_data_to_arduino(serial, hwall_matrix, vwall_matrix, start_cell_row, start_cell_col, heading)

    # if k & 0xFF == ord('v'):
    #     send_data_to_arduino_testing_lee(serial)
    #
    # if k & 0xFF == ord('t'):
    #     send_data_to_arduino_testing_vbc(serial, hwall_matrix, vwall_matrix, start_cell_row, heading, ard)
    # if k & 0xFF == ord('f'):
    #     generate_control_command(serial)

    # i += 1

    track_red(frame)

print("connection loss")
# release the camera
camera.release()
# delete all windows
cv2.destroyAllWindows()

