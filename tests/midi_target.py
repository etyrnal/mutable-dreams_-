"""No MIDI hardware: reject absent/duplicate targets before any port can open."""
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from midi_target import select_port,DEFAULT_PORT
class Ports:
 def __init__(self,ins,outs):self.ins=ins;self.outs=outs
 def get_input_names(self):return self.ins
 def get_output_names(self):return self.outs
p=Ports(['X0X FM-1',DEFAULT_PORT],['X0X FM-1',DEFAULT_PORT])
assert select_port(p)==DEFAULT_PORT
assert select_port(p,'X0X FM-1')=='X0X FM-1'
for ins,outs in [(['X0X FM-1'],['X0X FM-1']),([DEFAULT_PORT]*2,[DEFAULT_PORT]),([DEFAULT_PORT],[DEFAULT_PORT]*2),([DEFAULT_PORT],[]),([DEFAULT_PORT+' 2'],[DEFAULT_PORT+' 2'])]:
 try:select_port(Ports(ins,outs))
 except ValueError:pass
 else:raise AssertionError('Ambiguous or missing target accepted')
print('PASS exact MIDI selection; two different firmware endpoints stay separate; missing and duplicate targets rejected without opening.')
