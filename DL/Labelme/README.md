## 1.前期准备

首先确保自己电脑完成anaconda环境的安装，最好为anaconda3。

## 2.创建虚拟环境

```cpp
conda create -n labelme python=3
```

## 3.激活环境

```c
conda activate labelme
```

## 4.安装相关依赖

```c
conda install pyqt
conda install pillow
```

## 5.labelme安装

```c
// 此处的labelme版本可根据官网已经更新的版本进行选择
conda install labelme=5.1.1
```

![image-20230304153709235](https://raw.githubusercontent.com/kurisaW/picbed/main/img/202303041537614.png)

出现上面的标志几位安装成功。这里建议大家先更换国内的镜像源，可以多选几个厂商的。

## 6.启动labelme

首先激活label虚拟环境：

```c
conda activate labelme
```

启动labelme：

```c
labelme
```

## 7.实现labelme批量json_to_dataset方法

由于labelme标注后会生成一个json文件，为了避免重复操作我们提供一种labelme批量json_to_dataset方法：

找到自己所创建的虚拟环境：`E:\Anaconda3\envs\labelme\Lib\site-packages\labelme\cli`下的**json_to_dataset.py**文件，双击打开更换内容为：

```py
import argparse
import json
import os
import os.path as osp
import warnings
 
import PIL.Image
import yaml
 
from labelme import utils
import base64
 
def main():
    warnings.warn("This script is aimed to demonstrate how to convert the\n"
                  "JSON file to a single image dataset, and not to handle\n"
                  "multiple JSON files to generate a real-use dataset.")
    parser = argparse.ArgumentParser()
    parser.add_argument('json_file')
    parser.add_argument('-o', '--out', default=None)
    args = parser.parse_args()
 
    json_file = args.json_file
    if args.out is None:
        out_dir = osp.basename(json_file).replace('.', '_')
        out_dir = osp.join(osp.dirname(json_file), out_dir)
    else:
        out_dir = args.out
    if not osp.exists(out_dir):
        os.mkdir(out_dir)
 
    count = os.listdir(json_file) 
    for i in range(0, len(count)):
        path = os.path.join(json_file, count[i])
        if os.path.isfile(path):
            data = json.load(open(path))
            
            if data['imageData']:
                imageData = data['imageData']
            else:
                imagePath = os.path.join(os.path.dirname(path), data['imagePath'])
                with open(imagePath, 'rb') as f:
                    imageData = f.read()
                    imageData = base64.b64encode(imageData).decode('utf-8')
            img = utils.img_b64_to_arr(imageData)
            label_name_to_value = {'_background_': 0}
            for shape in data['shapes']:
                label_name = shape['label']
                if label_name in label_name_to_value:
                    label_value = label_name_to_value[label_name]
                else:
                    label_value = len(label_name_to_value)
                    label_name_to_value[label_name] = label_value
            
            # label_values must be dense
            label_values, label_names = [], []
            for ln, lv in sorted(label_name_to_value.items(), key=lambda x: x[1]):
                label_values.append(lv)
                label_names.append(ln)
            assert label_values == list(range(len(label_values)))
            
            lbl = utils.shapes_to_label(img.shape, data['shapes'], label_name_to_value)
            
            captions = ['{}: {}'.format(lv, ln)
                for ln, lv in label_name_to_value.items()]
            lbl_viz = utils.draw_label(lbl, img, captions)
            
            out_dir = osp.basename(count[i]).replace('.', '_')
            out_dir = osp.join(osp.dirname(count[i]), out_dir)
            if not osp.exists(out_dir):
                os.mkdir(out_dir)
 
            PIL.Image.fromarray(img).save(osp.join(out_dir, 'img.png'))
            #PIL.Image.fromarray(lbl).save(osp.join(out_dir, 'label.png'))
            utils.lblsave(osp.join(out_dir, 'label.png'), lbl)
            PIL.Image.fromarray(lbl_viz).save(osp.join(out_dir, 'label_viz.png'))
 
            with open(osp.join(out_dir, 'label_names.txt'), 'w') as f:
                for lbl_name in label_names:
                    f.write(lbl_name + '\n')
 
            warnings.warn('info.yaml is being replaced by label_names.txt')
            info = dict(label_names=label_names)
            with open(osp.join(out_dir, 'info.yaml'), 'w') as f:
                yaml.safe_dump(info, f, default_flow_style=False)
 
            print('Saved to: %s' % out_dir)
if __name__ == '__main__':
    main()
```

