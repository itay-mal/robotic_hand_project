from sklearn.tree import _tree
from sklearn.ensemble import RandomForestClassifier
from sklearn.tree import export_text
import numpy as np
import pickle
from typing import List

def main():
    with open("code/ML/saved_RF_calssifier_2axis.pickle", 'rb') as infile:
        model: RandomForestClassifier = pickle.load(infile)

    for idx, t in enumerate(model.estimators_):
        print(f'// tree {idx}')
        tree_rules = export_text(t, feature_names=[f'in[{i}]' for i in range(t.tree_.n_features)])
        # print(tree_rules)
        convert_tree_to_c(tree_rules.strip().split('\n'))
              
def convert_tree_to_c(rules: List[str] , var_pref='in'):
    level_p = 0
    rules.append('|')
    for idx, line in enumerate(rules):
        level = line.count('|') - 1
        if level < level_p:
            for l in reversed(range(level,level_p)):
                print('    ' * l + '}')
        print('    ' * level, end='')
        if 'class' in line:
            class_ = int(float(line.split(':')[-1].strip()))
            print(f'cls_hist[{class_}]++;')
        else:
            if '<=' in line:
                cond = line.split('---')[-1].strip()
                print(f'if({cond})' + ' {')
            elif '>' in line:
                print('else {')
        level_p = level
    print('')


if __name__ == '__main__':
    main()