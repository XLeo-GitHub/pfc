# -*- coding: utf-8 -*-
from imutils import perspective
from imutils import contours
import numpy as np
import argparse
import imutils
import sys
import cv2
from matplotlib import pyplot as plt
from pprint import pprint
import time
from scipy.spatial import distance as dist


class pfc_cv_measurer:
	COIN_PX = None
	COIN_MM = None
	MAX_CONTOURS = None
	ORIGINAL_FILE_PATH = None
	IMAGES = {
							'ORIGINAL' : None,
							'GRAY_IMG': None,
							'GAUSSIAN_IMG' : None,
							'CANNY_IMG' : None,
							'DILATE_IMG' : None,
							'ERODE_IMG' : None,
							'CV_IMG' : None
						}
	THRESHOLD_CONTOUR_AREA = 200
	THRESHOLD_MINIMUM_SIDE = 30
	PX_MM_RATIO = 0

	# 클래스 인스턴스시에 이미 CV 프로세스를 진행할 이미지가 접근가능하다.
	# CV 간에 참조해야하는 수치를 인스턴스시에 생성자의 매개변수로 전달받는다.
	def __init__(self,coin_px=10,coin_mm=10,max_contours=20,opath=None,carea=200,min_side=30):

		if opath == None:
			return False
			sys.exit()

		self.COIN_PX = coin_px
		self.COIN_MM = coin_mm
		self.MAX_CONTOURS = max_contours
		self.ORIGINAL_FILE_PATH = opath
		self.THRESHOLD_CONTOUR_AREA = carea
		self.THRESHOLD_MINIMUM_SIDE = min_side

		self.PX_MM_RATIO = self.calc_PxForMmRatio(self.COIN_PX,self.COIN_MM)



		self.transition_images()
		self.find_contours()
		self.save_images()

	# CV에 의해 식별된 Contour의 각 좌표를 순회하며, 해당 좌표가 대표하는 위치를 찾는다. 아래의 4개 위치 식별.
	# 1.TOP LEFT
	# 2.TOP RIGHT
	# 3.BOTTOM LEFT
	# 4.BOTTOM RIGHT
	# 위 식별된 4개의 점을 기준으로 관측된 식물의 Height / Widht 의 위치를 추론한다.
	def get_order_points(self,pts):
		xSorted = pts[np.argsort(pts[:,0]),:]
		leftMost = xSorted[:2,:]
		rightMost = xSorted[2:,:]

		leftMost = leftMost[np.argsort(leftMost[:,1]),:]
		(tl,bl) = leftMost

		D = dist.cdist(tl[np.newaxis], rightMost, "euclidean")[0]
		(br,tr) = rightMost[np.argsort(D)[::-1],:]

		return np.array([tl,tr,br,bl], dtype="float32")


	# 주어진 두 점의 좌표의 가운데 좌표를 반환한다.(좌표평면 기준)
	def get_midpoint(self,ptA,ptB):
		return ((ptA[0] + ptB[0]) * 0.5, (ptA[1] + ptB[1]) * 0.5)

	# 주어진 이미지의 GRAY/GAUSSIAN/CANNY/DILATE/ERODE 의 각 전처리를 수행한다.
	def transition_images(self):

		# 프로세싱처리가 필요한 오리지널 이미지 읽기
		self.IMAGES['ORIGINAL'] = cv2.imread(self.ORIGINAL_FILE_PATH)
		self.IMAGES['CV_IMG'] = self.IMAGES['ORIGINAL'].copy()
		self.IMAGES['GRAY_IMG'] = cv2.cvtColor(self.IMAGES['ORIGINAL'], cv2.COLOR_BGR2GRAY)
		self.IMAGES['GAUSSIAN_IMG'] = cv2.GaussianBlur(self.IMAGES['GRAY_IMG'], (1,1),0)
		self.IMAGES['CANNY_IMG'] = cv2.Canny(self.IMAGES['GAUSSIAN_IMG'],30,40)
		self.IMAGES['DILATE_IMG'] = cv2.dilate(self.IMAGES['CANNY_IMG'],None,iterations=2)
		self.IMAGES['ERODE_IMG'] = cv2.erode(self.IMAGES['DILATE_IMG'],None,iterations=1)



	# 주어진 PX(픽셀)과 MM(밀리미터)의 비율을 반환한다.
	def calc_PxForMmRatio(self,px, mm):
		return round(mm/px,7)

	# 이미지의 Contour 를 찾는다.
	def find_contours(self):
		copy_image = self.IMAGES['ERODE_IMG'].copy()
		f_contours = cv2.findContours(copy_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

		f_contours = f_contours[0] if imutils.is_cv2() else f_contours[1]
		(f_contours,_) = contours.sort_contours(f_contours)


		for (i,c) in enumerate(f_contours):
			box = cv2.minAreaRect(c)
			box = cv2.cv.BoxPoints(box) if imutils.is_cv2() else cv2.boxPoints(box)
			box = self.get_order_points(box)

			(tl,tr,br,bl) = box
			if dist.euclidean(tl,tr) < self.THRESHOLD_MINIMUM_SIDE or dist.euclidean(tl,bl) < self.THRESHOLD_MINIMUM_SIDE:
				continue
			if cv2.contourArea(c) < self.THRESHOLD_CONTOUR_AREA :
				continue


			# 주어진 조건을 통과한 Contour 이미지 삽입
			cv2.drawContours(self.IMAGES['CV_IMG'], [box.astype("int")], -1, (144,43,120),8)
			# 박스의 각 모서리 위치의 좌표에 원형 이미지 삽입
			for i, (x,y) in enumerate(box):
				cv2.circle(self.IMAGES['CV_IMG'],(int(x), int(y)), 2, (0,0,255), -1)

			(tl,tr,br,bl) = box
			(tltrX, tltrY) = self.get_midpoint(tl,tr)
			(blbrX, blbrY) = self.get_midpoint(bl,br)
			(tlblX, tlblY) = self.get_midpoint(tl,bl)
			(trbrX, trbrY) = self.get_midpoint(tr,br)

			cv2.circle(self.IMAGES['CV_IMG'], (int(tltrX), int(tltrY)),10,(255,0,0),-1)
			cv2.circle(self.IMAGES['CV_IMG'], (int(blbrX), int(blbrY)),10,(255,0,0),-1)
			cv2.circle(self.IMAGES['CV_IMG'], (int(tlblX), int(tlblY)),10,(255,0,0),-1)
			cv2.circle(self.IMAGES['CV_IMG'], (int(trbrX), int(trbrY)),10,(255,0,0),-1)

			cv2.line(self.IMAGES['CV_IMG'], (int(tltrX),int(tltrY)), (int(blbrX),int(blbrY) ), (255,0,0),8)
			cv2.line(self.IMAGES['CV_IMG'], (int(tlblX), int(tlblY)),(int(trbrX), int(trbrY)), (255,0,0),8)

			# 측정된 PX 길이 측정 및 mm(밀리미터) 단위 변환
			dA = dist.euclidean((tltrX, tltrY), (blbrX, blbrY))
			dB = dist.euclidean((tlblX, tlblY), (trbrX, trbrY))

			mid_point_dot = tuple(map(lambda x: int(x), self.get_midpoint((tlblX, tlblY), (trbrX, trbrY))))

			cv2.putText(self.IMAGES['CV_IMG'], str(round(self.PX_MM_RATIO * dA,2)) + "mm", (mid_point_dot[0]-60,mid_point_dot[1]-60), cv2.FONT_HERSHEY_SIMPLEX,3, (102,255,255),5)
			cv2.putText(self.IMAGES['CV_IMG'], str(round(self.PX_MM_RATIO * dB,2)) + "mm", (mid_point_dot[0]-60,mid_point_dot[1]-60),cv2.FONT_HERSHEY_SIMPLEX,3, (102,255,255),5)



	# 최종작업을 완료하고 모든 이미지들을 저장한다.
	def save_images(self):
		splt_image_file_name = self.ORIGINAL_FILE_PATH.split('.')
		cv2.imwrite(splt_image_file_name[0] + "_cv." + splt_image_file_name[1], self.IMAGES['CV_IMG'])


if __name__ == '__main__':
	pfc_cv_measurer = pfc_cv_measurer(coin_px=225.79, coin_mm=24, max_contours=20, opath="/Users/house/DEV/PYTHON/openag/images/OG_PHOTO_20171101_141941.jpg",carea=200, min_side=30)





















