/*------------------------------------------------ ASSEMBLER RAW ----------------------------------------------*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;

namespace GenesisExplorer
{

    class Assembler
    {
        public static byte[] map = new byte[256];
        public static byte[] numap = new byte[256];
        public static string[] mnemo = new string[256];
        public static byte[] gazmap = new byte[256];

        public static List<string> logR = new List<string>();
        public static List<string> logD = new List<string>();

        public static byte[] BIN;
        public static int bOffset;

        public static void GenerateOPCODEMapping() // garbagge but ok 
        {

            for (int i = 0; i < 256; i++)
            {
                map[i] = 255; // the HLT OPCODE
                numap[i] = 0;
            }
            string fpath = "vmopcode1.ini";
            string[] fs = File.ReadAllLines(fpath);
            uint opcnt = 0;
            uint tot = 0;
            foreach (string s in fs)
            {
                string[] fparse = s.Split('-');
                if (fparse.Length == 3)
                {
                    tot++;
                    int bitnum = int.Parse(fparse[1]);
                    int gas = int.Parse(fparse[2]);
                    if (bitnum == 3)
                    {
                        byte opc, opb;
                        bool _f;

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, true, ref opb);
                            SetBit(6, true, ref opb);
                            SetBit(5, true, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, true, ref opb);
                            SetBit(6, false, ref opb);
                            SetBit(5, true, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, true, ref opb);
                            SetBit(6, false, ref opb);
                            SetBit(5, false, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, false, ref opb);
                            SetBit(6, false, ref opb);
                            SetBit(5, false, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, true, ref opb);
                            SetBit(6, true, ref opb);
                            SetBit(5, false, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, false, ref opb);
                            SetBit(6, true, ref opb);
                            SetBit(5, false, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, false, ref opb);
                            SetBit(6, false, ref opb);
                            SetBit(5, true, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, false, ref opb);
                            SetBit(6, true, ref opb);
                            SetBit(5, true, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                    }
                    if (bitnum == 2)
                    {

                        byte opc, opb;
                        bool _f;

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, true, ref opb);
                            SetBit(5, true, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, true, ref opb);
                            SetBit(5, false, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, false, ref opb);
                            SetBit(5, true, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }

                        opc = 0;
                        opb = 0;
                        _f = false;
                        while (opc < 255)
                        {

                            SetBit(7, false, ref opb);
                            SetBit(5, false, ref opb);
                            if (map[opb] == 255 && opb != 255)
                            {
                                map[opb] = (byte)opcnt;
                                numap[opb] = (byte)bitnum;
                                gazmap[opb] = (byte)gas;
                                _f = true;
                                break;
                            }
                            opc++;
                            opb = opc;
                        }
                        if (!_f) { MessageBox.Show("shit " + opcnt.ToString()); }
                    }
                    if (bitnum == 0)
                    {
                        bool _f = false;
                        for (int i = 0; i < 255; i++)
                        {
                            if (map[i] == 255)
                            {
                                map[i] = (byte)opcnt;
                                numap[i] = 0; // <--- 0 nu map not workin
                                gazmap[i] = (byte)gas;
                                _f = true;
                                break;
                            }
                        }
                        if (!_f)
                        {
                            MessageBox.Show("shit here"); return;
                        }
                    }
                    // last 3 bits will be equal to 
                    mnemo[opcnt] = fparse[0].Replace(" ", "").ToLower();
                    opcnt++;
                }

                else
                {
                    MessageBox.Show("bad parsing " + fparse.Length.ToString());
                }

            }

        }

        public static void PrintOPMaps()
        {
            string mapstr = "";
            foreach (byte b in map)
            {
                mapstr += b.ToString() + ", ";
            }
            MessageBox.Show("OPCODE MAP: \r\n" + mapstr);

            mapstr = "";
            foreach (byte b in numap)
            {
                mapstr += b.ToString() + ", ";
            }
            MessageBox.Show("OPERAND SIZE MAP: \r\n" + mapstr);
            mapstr = "";
            foreach (byte b in gazmap)
            {
                mapstr += b.ToString() + ", ";
            }
            MessageBox.Show("GAS PRICE MAP: \r\n" + mapstr);
        }

        public class Label
        {
            public string name { get; set; }
            public int pos { get; set; }
            public int spos { get; set; }
            public bool _subLabel { get; set; }

            public string _mainreference { get; set; }
            public Label(string n, int p, int strpos, bool _sub, string mainRef = "")
            {
                this.name = n;
                this.pos = p;
                this.spos = strpos;
                this._subLabel = _sub;

                if (_sub)
                {
                    _mainreference = mainRef;
                }
            }
        }


        public class Ref
        {
            public int LabelIndex { get; set; }
            public int Pos { get; set; }
            public bool _dsp { get; set; }
            public bool _sib { get; set; }
            public bool change_imm { get; set; }
            public int spos { get; set; }
            public Ref(int index, int p, bool isImm, int strpos)
            {
                this.LabelIndex = index;
                this.Pos = p;
                this.change_imm = isImm;
                this._sib = false;
                this._dsp = false;
                this.spos = strpos;


            }
        }

        public static List<Label> labels = new List<Label>();
        public static List<Ref> Refs = new List<Ref>(); // is byte and is 
        public static List<int> Entries = new List<int>();


        public static void CreateCRT(string[] txt)
        {
            int blocindex = -1;
            int txindex = -1;
            int entryindex = -1;
            List<uint> pushOps = new List<uint>();

            foreach (string s in txt)
            {
                if (s.Contains("###"))
                {
                    int.TryParse(s.Replace("###", "").Replace(" ", ""), out blocindex);
                }
                else if (s.Contains("##"))
                {
                    int.TryParse(s.Replace("##", "").Replace(" ", ""), out txindex);
                }
                else if (s.Contains("#"))
                {
                    int.TryParse(s.Replace("#", "").Replace(" ", ""), out entryindex);
                }
                else
                {
                    uint n = 0;
                    if (uint.TryParse(s.Replace(" ", ""), out n))
                    {
                        pushOps.Add(n);
                    }
                }
            }
            if (blocindex == -1)
            {
                MessageBox.Show("Bloc index was not declared. Use ### to declare it.");
                return;
            }
            if (txindex == -1)
            {
                MessageBox.Show("Transaction index was not declared. Use ## to declare it.");
                return;
            }
            if (entryindex == -1)
            {
                MessageBox.Show("Entry index was not declared. Use # to declare it.");
                return;
            }
            //WE NEED TO SIGN now ... 
            MessageBox.Show("[WARNING] This will create a CRT file with blank header. Cannot be mined. Only for testing purpose.");
            List<byte> data = new List<byte>();

            // fill blank header
            for (int i = 0; i < 81; i++) // last four bytes are uint of txdata size
            {
                data.Add(0);
            }
            uint datasize = 16 + (4 * (uint)pushOps.Count);
            AddBytesToList(ref data, BitConverter.GetBytes(datasize));

            AddBytesToList(ref data, BitConverter.GetBytes(blocindex));
            AddBytesToList(ref data, BitConverter.GetBytes(txindex));
            for (int i = 0; i < pushOps.Count; i++)
            {
                AddBytesToList(ref data, BitConverter.GetBytes(pushOps[i]));
            }
            AddBytesToList(ref data, BitConverter.GetBytes(entryindex));
            AddBytesToList(ref data, BitConverter.GetBytes(uint.MaxValue));


            Stream myStream;
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();

            saveFileDialog1.Filter = "All files (*.*)|*.*";
            saveFileDialog1.FilterIndex = 2;
            saveFileDialog1.RestoreDirectory = true;

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if ((myStream = saveFileDialog1.OpenFile()) != null)
                {

                    myStream.Write(ListToByteArray(data), 0, data.Count);
                    myStream.Close();
                }
            }
        }
        public static void ShowMnemoOpcodes(string t)
        {
            int idx = -1;
            for (int i = 0; i < mnemo.Length; i++)
            {
                if (mnemo[i] == t)
                {
                    idx = i;
                }
            }
            if (idx == -1) { return; }
            string opcodes = "";
            for (int i = 0; i < map.Length; i++)
            {
                if (map[i] == idx)
                {
                    opcodes += i.ToString("X") + " ,";
                }
            }
            MessageBox.Show(t + ":" + opcodes);
        }

        public static void PrintLabelsPosition()
        {
            string s = "";
            foreach (Label l in labels)
            {
                s += l.name + " : " + l.pos;
                if (l._subLabel)
                {
                    s += "[" + l._mainreference + "]";
                }
                s += "\r\n";
            }
            MessageBox.Show(s);
        }
        public static void PrintRefInfo()
        {
            string s = "";
            foreach (Ref r in Refs)
            {
                s += "[ref info]";
                s += "\r\n";
                s += "line#  : " + r.spos;
                s += "\r\n";
                s += "labelname : " + labels[r.LabelIndex].name;
                s += "\r\n";
                s += "byte pos  : " + r.Pos;
                s += "\r\n";
            }
            MessageBox.Show(s);
        }
        public static void ProccessText(string[] txt, bool _save = false, bool _printLog = false)
        {
            // RUN IT TWICE FOR LABEL BECAUSE WE DONT GIVE A SHIT
            labels = new List<Label>();
            Refs = new List<Ref>();
            bOffset = 0;
            logR = new List<string>();
            logD = new List<string>();
            Entries = new List<int>();



            // [0] PRE PROCESS LABELS
            for (int i = 0; i < txt.Length; i++)
            {
                PreProcessLabel(txt[i], i);
            }
            int entrycount = GetEntryCount(txt);
            // [1] CONVERT
          

            List<byte> binlst = new List<byte>();

            List<string> LOG = new List<string>();
            
            for (int i = 0; i < txt.Length; i++)
            {
                byte[] B = ConvertLineToBinary(txt[i], entrycount, i);
                if (B != null)
                {
                    bOffset += B.Length;
                    AddBytesToList(ref binlst, B);
                    if (_printLog)
                    {
                        string str = "Proccessing " + txt[i] + ":";
                        foreach (byte b in B)
                        {
                            str += ((int)b).ToString() + " ";
                        }
                        LOG.Add(str);
                    }
                   
                }
                else
                {

                    //  logD.Add("invalid : " + s);
                }
            }
            
            string dumpstr = "";
            foreach (byte b in binlst)
            {
                dumpstr += b.ToString("X") + " ";
                dumpstr += "(" + GetByteString(b) + ")";

            }

            BIN = ListToByteArray(binlst);

            // [3] POST PROCESS LABEL
            PostProcessLabel();
            if (_printLog)
            {
                List<string> RAWLOG = new List<string>();
                string s = " ";
                int jctr = 0;
                foreach (byte b in BIN)
                {
                    s += ((int)b).ToString() + " ";
                    jctr++;
                    if (jctr > 12)
                    {
                        RAWLOG.Add(s);
                        s = "";
                        jctr = 0;
                    }
                }
                File.WriteAllLines("C:\\Users\\gaelg\\Desktop\\LOG.txt", LOG.ToArray());
                File.WriteAllLines("C:\\Users\\gaelg\\Desktop\\RAWLOG.txt", RAWLOG.ToArray());
                
                PrintRefInfo();
            }
            PrintLabelsPosition();

            // [4] CONCAT WITH ENTRIES

            List<byte> flist = new List<byte>();
            AddBytesToList(ref flist, BitConverter.GetBytes((uint)Entries.Count));
            foreach (int i in Entries)
            {
                AddBytesToList(ref flist, BitConverter.GetBytes((uint)i));
            }
            foreach (byte b in BIN)
            {
                flist.Add(b);
            }
            BIN = ListToByteArray(flist);




            // [opt.] Print Memory Dump


            logD.Add(dumpstr);

            if (_save)
            {
                MessageBox.Show("[WARNING] This will create a CST file with blank header. Cannot be mined. Only for testing purpose.");
                Stream myStream;
                SaveFileDialog saveFileDialog1 = new SaveFileDialog();

                List<byte> outb = new List<byte>();
                for (int i = 0; i < 85; i++)
                {
                    outb.Add(0);
                }
                AddBytesToList(ref outb, BIN);

                saveFileDialog1.Filter = "All files (*.*)|*.*";
                saveFileDialog1.FilterIndex = 2;
                saveFileDialog1.RestoreDirectory = true;

                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    if ((myStream = saveFileDialog1.OpenFile()) != null)
                    {

                        myStream.Write(ListToByteArray(outb), 0, outb.Count);
                        myStream.Close();
                    }
                }
            }
            // [2] this was just it.
        }

        public static int GetEntryCount(string[] text)
        {
            int result = 0;
            for (int i = 0; i < text.Length; i++)
            {
                string s = text[i];
                s = s.Replace("\r\n", "");
                s = s.Replace("\r", "");

                int eindex = s.IndexOf("#");
                if (eindex > -1)
                {
                    string entry = s.Substring(eindex + 1, s.Length - (eindex + 1));
                    entry = entry.Replace(" ", "");
                    foreach (Label l in labels)
                    {
                        if (l.name == entry)
                        {
                            result++;
                            break;
                        }
                    }

                }
            }
            return result;
        }
        public static int DetectFormat(string operand)
        {

            int val;

            for (int i = 0; i < operand.Length; i++)
            {
                // does it direct value 
                if (int.TryParse(operand, out val))
                {
                    logR.Add("Immediate addressing"); // OK BUT IT IS 4 BYTES OR ONE ???
                    return 0;
                }

                if (GetReg_REGRM(operand) != null)
                {
                    logR.Add("Direct Register addressing");
                    return 1;
                }


                // if contains labels or dd : Raw indirect value addressing
                if (operand.Length >= 3)
                {
                    if (operand[0] == '[' && operand[operand.Length - 1] == ']')
                    {

                        string ioperand = operand.Substring(1, operand.Length - 2);


                        if (GetReg_REGRM(ioperand) != null)
                        {
                            logR.Add("Indirect Register addressing");
                            return 2;
                        }
                        if (int.TryParse(ioperand, out val))
                        {
                            logR.Add("Raw indirect value addressing [disp only 32]");
                            return 3;
                        }

                        int Plus = ioperand.IndexOf('+');
                        int Minus = ioperand.IndexOf('-');
                        if (Minus > -1 && Plus == -1)
                        {
                            string[] splitmin = ioperand.Split('-');
                            if (splitmin.Length != 2)
                            {
                                return -1; //eror
                            }
                            if (GetReg_REGRM(splitmin[0]) != null && int.TryParse(splitmin[1], out val))
                            {
                                if (val > 255)
                                {
                                    logR.Add("Indirect + DISP32 addressing");
                                    return 4;
                                }
                                else
                                {
                                    logR.Add("Indirect + DISP8 addressing");
                                    return 5;
                                }

                            }

                        }
                        if (Plus > -1 && Minus == -1)
                        {
                            // DETECT IF SIB
                            string[] splitplus = ioperand.Split('+');
                            if (splitplus.Length != 2)
                            {
                                return -1; //eror
                            }
                            if (ContainsRegister(splitplus[0], true) > -1 && ContainsRegister(splitplus[1], true) > -1)
                            {
                                logR.Add("SIB DETECTED"); // !!! 
                                return 6;
                            }
                            if (ContainsRegister(splitplus[0], true) > -1 && splitplus[0].Contains('*'))
                            {
                                logR.Add("SIB DETECTED"); // !!! 
                                return 6;
                            }
                            if (ContainsRegister(splitplus[1], true) > -1 && splitplus[1].Contains('*'))
                            {
                                logR.Add("SIB DETECTED"); // !!! 
                                return 6;
                            }
                            else
                            {
                                // probably just a disp
                                if (GetReg_REGRM(splitplus[0]) != null && int.TryParse(splitplus[1], out val))
                                {
                                    if (val > 255)
                                    {
                                        logR.Add("Indirect + DISP32 addressing");
                                        return 7;
                                    }
                                    else
                                    {
                                        logR.Add("Indirect + DISP8 addressing");
                                        return 8;
                                    }

                                }
                                if (GetReg_REGRM(splitplus[1]) != null && int.TryParse(splitplus[0], out val))
                                {
                                    if (val > 255)
                                    {
                                        logR.Add("Indirect + DISP32 addressing");
                                        return 9;
                                    }
                                    else
                                    {
                                        logR.Add("Indirect + DISP8 addressing");
                                        return 10;
                                    }

                                }
                            }

                        }
                        //int Plus = ins.IndexOf('+');
                    }
                }
            }
            return -1; //eror

        }

        public static int DetectDepth(string operand)
        {
            byte[] RR = GetReg_REGRM(operand);
            if (RR != null)
            {
                return RR[1];
            }
            // quand c'est immediat c'est -1. quand 
            if (operand.Contains("dword"))
            {
                return 32;
            }
            if (operand.Contains("word"))
            {
                return 16;
            }
            if (operand.Contains("byte"))
            {
                return 8;
            }
            return -1;
        }



        public static void SortLabelByNameLength()
        {
            IEnumerable<Label> query = labels.OrderBy(x => x.name.Length); // x is element i mean it is ok 
            List<Label> nlabels = new List<Label>();
            for (int i = query.Count() - 1; i >= 0; i--)
            {
                nlabels.Add(query.ElementAt(i));
            }
            labels = nlabels;
        }
        public static int GetMainLabelFromSubPosition(int subpos)
        {
            int result = -1;
            int bestpos = -1;
            for (int i = 0; i < labels.Count; i++)
            {
                if (labels[i].spos >= subpos)
                {
                    break;
                }
                if (!labels[i]._subLabel)
                {

                    if (labels[i].spos > bestpos)
                    {
                        result = i;
                        bestpos = labels[i].spos;
                    }
                }
            }

            return result;
        }

        public static void PreProcessLabel(string s, int spos)
        {
            s = s.Replace("\r\n", "");
            s = s.Replace("\r", "");
            s = s.Split(';')[0];

            string[] labelsplit = s.Split(':');
            if (labelsplit.Length > 1)
            {
                if (labelsplit[0][0] == '.')
                {
                    // get the closest non sub label near low boff
                    int mlabel = GetMainLabelFromSubPosition(spos);
                    if (mlabel > -1)
                    {
                        //MessageBox.Show("should reference " + labelsplit[0]+  " : " + labels[mlabel].name );
                        labelsplit[0] = labelsplit[0].Replace(" ", "");
                        labels.Add(new Label(labelsplit[0], bOffset, spos, true, labels[mlabel].name));
                    }
                    else
                    {
                        logR.Add("Cannot found main label from sub label at " + bOffset.ToString());
                    }

                }
                else
                {
                    labels.Add(new Label(labelsplit[0], bOffset, spos, false));
                }

                // sort labels by length name 
                //  MessageBox.Show("adding " + labelsplit[0].Replace(" ",""));
                // s = labelsplit[1];

            }

            bool def = false;
            int dpos = -1;
            dpos = s.IndexOf(" dd ");
            if (dpos > -1)
            {
                def = true;
            }
            else
            {
                dpos = s.IndexOf(" dw ");
                if (dpos > -1 && !def)
                {
                    def = true;
                }
                else
                {
                    dpos = s.IndexOf(" db ");
                    if (dpos > -1)
                    {
                        def = true;
                    }

                }
            }

            if (def)
            {
                if (dpos > 0)
                {
                    string sname = s.Substring(0, dpos);
                    sname = sname.Replace(" ", "");
                    labels.Add(new Label(sname, bOffset, spos, false));
                    // SHOULD I DO SUB REFERENCEMENT FOR DEFINE DATA?
                }
            }
            SortLabelByNameLength();
        }

        public static void PostProcessLabel()
        {
            foreach (Ref r in Refs) // ref is always 4 bytes .... 
            {
                byte[] nval = BitConverter.GetBytes(labels[r.LabelIndex].pos);
                // logD.Add(labels[r.LabelIndex].name + " offset is " + labels[r.LabelIndex].pos.ToString());
                byte modo = 2;
                if (r._sib)
                {
                    modo++;
                }
                if (!r.change_imm)
                {

                    BIN[r.Pos + modo] = nval[0];
                    BIN[r.Pos + modo + 1] = nval[1];
                    BIN[r.Pos + modo + 2] = nval[2];
                    BIN[r.Pos + modo + 3] = nval[3];
                }
                else
                {
                    if (r._dsp)
                    {
                        modo++;
                    }

                    BIN[r.Pos + modo] = nval[0];
                    BIN[r.Pos + modo + 1] = nval[1];
                    BIN[r.Pos + modo + 2] = nval[2];
                    BIN[r.Pos + modo + 3] = nval[3];
                }


            }
        }
        public static byte[] ConvertLineToBinary(string s, int entrycount, int spos)
        {
            s = s.Replace("\r\n", "");
            s = s.Replace("\r", "");
            s = s.Split(';')[0];

            if (s.Length == 0)
            {
                return null;
            }
            // ________________ PROCESS LABEL UPDATE__________________

            // process label
            string[] labelsplit = s.Split(':');
            if (labelsplit.Length > 1)
            {

                foreach (Label l in labels)
                {
                    if (l.name == labelsplit[0].Replace(" ", "")) // here there is trouble .... 
                    {
                        //UPDATE pos if from main label reference...
                        if (l._subLabel)
                        {
                            int mlabel = GetMainLabelFromSubPosition(spos);
                            if (mlabel > -1)
                            {
                                if (l._mainreference == labels[mlabel].name)
                                {
                                    l.pos = bOffset + 0x5d + 4 + (4 * entrycount);
                                    break;
                                }
                            }


                        }
                        else
                        {
                            l.pos = bOffset + 0x5d + 4 + (4 * entrycount);
                            break;

                        }

                    }


                }
                s = labelsplit[1];

            }
            // ________________     Entries     __________________
            int eindex = s.IndexOf("#");
            if (eindex > -1)
            {
                string entry = s.Substring(eindex + 1, s.Length - (eindex + 1));
                entry = entry.Replace(" ", "");
                foreach (Label l in labels)
                {
                    if (l.name == entry)
                    {
                        Entries.Add(l.pos);
                        logD.Add("adding entry at " + l.pos);
                        break;
                    }
                }

            }

            // ________________ PROCESS DEFINE __________________

            /*
              define format are :  
              name type value 
              name type value times value
              name type value, value, value, value
              type value
              type value, value, value ... etc.

             */
            // get type.
            int dtype = 0;
            bool def = false;
            int dpos = -1;
            dpos = s.IndexOf(" dd ");
            if (dpos > -1)
            {
                def = true;
                dtype = 32;
            }
            else
            {
                dpos = s.IndexOf(" dw ");
                if (dpos > -1 && !def)
                {
                    def = true;
                    dtype = 16;
                }
                else
                {
                    dpos = s.IndexOf(" db ");
                    if (dpos > -1)
                    {
                        def = true;
                        dtype = 8;
                    }

                }

            }

            if (def)
            {
                if (dpos > 0)
                {
                    // i dont need to care about subreferencement for define values cause it is not implemented ... 
                    // do not need to update cause its a definition 
                    string sname = s.Substring(0, dpos);
                    //  labels.Add(new Label(sname, bOffset));
                    //  MessageBox.Show("adding def name " + sname);
                    foreach (Label l in labels)
                    {
                        if (l.name == sname.Replace(" ", ""))
                        {
                            l.pos = bOffset + 0x5d + 4 + (4 * entrycount); ;
                            break;
                        }
                    }


                }

                s = s.Substring(dpos + 4, s.Length - (dpos + 4)); // suppress name and dd.
                string[] splitdef = s.Split(',');
                if (splitdef.Length == 1)
                {
                    // check if times 
                    int tindex = splitdef[0].IndexOf("times");
                    int times = 0;
                    int value;
                    if (tindex > -1)
                    {
                        if (!int.TryParse(splitdef[0].Substring(0, tindex), out value))
                        {
                            return null;
                        }
                        if (!int.TryParse(splitdef[0].Substring(tindex + 5, splitdef[0].Length - 5 - tindex), out times))
                        {
                            return null;
                        }

                        // adding the  fucking byte 
                        List<byte> defbytes = new List<byte>();
                        for (int i = 0; i < times; i++)
                        {
                            if (dtype == 8)
                            {
                                defbytes.Add((byte)value);
                            }
                            if (dtype == 16)
                            {
                                AddBytesToList(ref defbytes, BitConverter.GetBytes((short)value));
                            }
                            if (dtype == 32)
                            {
                                AddBytesToList(ref defbytes, BitConverter.GetBytes(value));
                            }
                        }
                        return ListToByteArray(defbytes);
                    }
                    else
                    {
                        if (!int.TryParse(splitdef[0], out value))
                        {
                            return null;
                        }
                        List<byte> defbytes = new List<byte>();
                        if (dtype == 8)
                        {
                            defbytes.Add((byte)value);
                        }
                        if (dtype == 16)
                        {
                            AddBytesToList(ref defbytes, BitConverter.GetBytes((short)value));
                        }
                        if (dtype == 32)
                        {
                            AddBytesToList(ref defbytes, BitConverter.GetBytes(value));
                        }
                        return ListToByteArray(defbytes);
                        // adding the fucking byte
                    }
                }
                else
                {
                    int value;
                    List<byte> defbytes = new List<byte>();
                    // parse , && adding fucking bytes 
                    for (int i = 0; i < splitdef.Length; i++)
                    {
                        if (!int.TryParse(splitdef[i], out value))
                        {
                            return null;
                        }

                        if (dtype == 8)
                        {
                            defbytes.Add((byte)value);
                        }
                        if (dtype == 16)
                        {
                            AddBytesToList(ref defbytes, BitConverter.GetBytes((short)value));
                        }
                        if (dtype == 32)
                        {
                            AddBytesToList(ref defbytes, BitConverter.GetBytes(value));
                        }

                    }
                    return ListToByteArray(defbytes);
                }


            }

            // [I.1] GET MNEMONIC OPCODE BYTE
            int ctr = 0;
            foreach (char c in s)
            {
                if (c == ' ')
                {
                    ctr++;
                }
                else
                {
                    break;
                }
            }

            s = s.Substring(ctr, s.Length - ctr);

            string[] parser = s.Split(' ');
            string opstr = parser[0];
            int mnemoid = -1;
            for (int i = 0; i < 255; i++)
            {
                if (opstr.ToLower() == mnemo[i])
                {
                    mnemoid = i;
                    break;
                }
            }

            int opnum = -1;
            for (int i = 0; i < map.Length; i++)
            {
                if (map[i] == mnemoid)
                {
                    opnum = numap[i];
                    break;
                }
            }
            if (opnum == -1)
            {
                logR.Add("Undefined Mnemonic.");
                return null;
            }


            byte opbyte = 255;

            if (opnum == 0)
            {

                for (int i = 0; i < map.Length; i++)
                {
                    if (map[i] == mnemoid)
                    {
                        opbyte = (byte)i;
                    }
                }
                logR.Add(opbyte.ToString());
                return new byte[1] { opbyte };
            }
            else
            {
                logR.Add("opnum : " + opnum.ToString() + " num id : " + mnemoid.ToString());

            }


            opnum--;

            // [1] CHECK IF NUMBER OF OPERANDS IS VALID 
            if (s.Length <= opstr.Length)
            {
                logR.Add("Bad operand size");
                return null;

            }


            string oprstr = s.Substring(opstr.Length, s.Length - opstr.Length);
            oprstr = oprstr.Replace(" ", "");
            parser = oprstr.Split(',');

            if (parser.Length != opnum)
            {
                return null; // BAD OPERANDS SIZE
            }

            // _______________________    DETECT         DEPTH    ______________________
            bool sbit = false;
            int rsize = -1;
            // get always the highest bit. 
            // IS IT MORE LOGIC TO JUST BASE DEPTH ON FIRST OPERAND ????
            for (int i = 0; i < parser.Length; i++)
            {
                int r = DetectDepth(parser[i]);
                if (r > -1 && r > rsize)
                {
                    rsize = r;
                }
                parser[i] = parser[i].Replace("dword", "");
                parser[i] = parser[i].Replace("word", "");
                parser[i] = parser[i].Replace("byte", "");
            }
            if (rsize == -1 && parser.Length == 2)
            {
                logR.Add("operand size not detected.");
                return null;
            }
            if (rsize == -1 && parser.Length == 1) // if one operand and ( like push, jmp etc. set it to 32 ) s
            {
                rsize = 32;
            }
            if (rsize == 8) // we do not process 16bit mode for the moment
            {
                rsize = 32;
            }
            if (rsize > 8)
            {
                sbit = true;
            }



            //--------------------- replace label name in operands by true values ------------------

            int[] opfrmt = new int[parser.Length];
            byte SIB = 255;
            byte[] SIBDSP = null;
            int refcount = 0;
            for (int i = 0; i < parser.Length; i++)
            {
                // if it contains a point 
                // PARSE LABEL
                for (int a = 0; a < labels.Count; a++)
                {

                    int search = -1;
                    int pointsearch = parser[i].IndexOf(".");
                    // does it contains sublabel reference ?
                    if (pointsearch > -1)
                    {

                        // labels a should be a sublabel and sublabel should reference same mainlabel string
                        int mlabel = GetMainLabelFromSubPosition(spos);

                        if (mlabel > -1)
                        {
                            if (labels[a]._subLabel && labels[a]._mainreference == labels[mlabel].name)
                            {

                                search = parser[i].IndexOf(labels[a].name);
                            }
                        }
                    }
                    else
                    {
                        search = parser[i].IndexOf(labels[a].name);

                    }
                    if (search == -1)
                    {
                        logR.Add("Cannot found main label from sub label reference at line " + spos);
                    }
                    // if it contains a point. do custom search. else 

                    // label should be sorted by name length to avoid conflict
                    if (search > -1)
                    {
                        if (labels[a]._subLabel)
                        {
                             MessageBox.Show("line #" + spos.ToString()  + ":" + s+" reference " + labels[a]._mainreference+labels[a].name + " at offset : " + bOffset);
                        }
                        else
                        {
                             MessageBox.Show("line #" + spos.ToString() + ":" + s + " reference " + labels[a].name + " at offset : " + bOffset);
                        }

                        // should be exactly the same name label. ( with no include) 
                        // recompose string 
                        string pA = "";
                        string pB = "";
                        pA = parser[i].Substring(0, search);
                        pB = parser[i].Substring(search + labels[a].name.Length, parser[i].Length - (search + labels[a].name.Length));
                        parser[i] = pA + "666" + pB;//s = pA + l.pos.ToString() + pB;

                        // add the ref. 
                        if (parser[i][0] == '[' && parser[i][parser[i].Length - 1] == ']')
                        {
                            Refs.Add(new Ref(a, bOffset, false, spos));
                        }
                        else
                        {
                            Refs.Add(new Ref(a, bOffset, true, spos));
                        }
                        refcount++;
                        break;
                    }
                }
                // _______________________    DETECT         FORMAT   ______________________
                opfrmt[i] = DetectFormat(parser[i]);

                if (opfrmt[i] == -1)
                    return null;

                if (opfrmt[i] == 0 && i == 0 && opnum == 2) // dont accept imm addr as first operand ??? 
                    return null;
                if (opfrmt[i] > 1 && opfrmt[i] != 6 && refcount > 0)
                {
                    Refs[Refs.Count - 1]._dsp = true;
                }

                // if 6, build sib byte ....
                if (opfrmt[i] == 6)
                {
                    if (refcount > 0)
                    {
                        Refs[Refs.Count - 1]._sib = true;
                    }
                    if (SIB != 255)
                    {
                        logR.Add("Error, 2 SIB mode not allowed");
                        return null; // DONT ALLOW 2 SIB
                    }
                    byte[] sr = GETSIB(parser[i]);
                    if (sr == null)
                    {
                        logR.Add("Error, SIB was null");
                        return null;
                    }
                    if (sr.Length == 5)
                    {
                        SIB = sr[0];
                        SIBDSP = new byte[4] { sr[1], sr[2], sr[3], sr[4] };
                        Refs[Refs.Count - 1]._dsp = true;
                    }
                    else
                    {
                        SIB = sr[0];
                    }

                    logR.Add("SIB : 0x" + SIB.ToString("X"));
                    //PrintByte(SIB);
                    if (SIBDSP != null)
                    {
                        logR.Add("SIB DSP : " + BitConverter.ToInt32(SIBDSP, 0).ToString());
                    }

                }
            }

            if (opfrmt.Length > 1)
            {
                if (opfrmt[0] > 1 && opfrmt[1] > 1) // do not  accept 2 indirect addressing ... 
                    return null;


                return Encode((byte)opfrmt[0], (byte)opfrmt[1], parser[0], parser[1], sbit, SIB, SIBDSP, mnemoid, spos);
            }
            else
            {
                return Encode((byte)opfrmt[0], 255, parser[0], "", sbit, SIB, SIBDSP, mnemoid, spos);
            }


        }

        public static byte[] GETSIB(string operand) // RETURN SIB BYTE && Return SIBDISP 
        {
            operand = operand.Substring(1, operand.Length - 2);
            string[] splitplus = operand.Split('+');

            byte SIB = 0;
            byte[] dsp;

            if (splitplus.Length != 2)
            {
                logR.Add("NO + IN SIB");
                return null;
            }


            // CHECK ALL 2 CAN BE PARSE
            byte L = GetReg_SIB(splitplus[0]);
            byte R = GetReg_SIB(splitplus[1]);

            if (L != 255 && R != 255)
            {
                // scale : 0 0, index : L, base : R 
                SetBit(0, false, ref SIB);
                SetBit(1, false, ref SIB);

                SetBit(2, IsBitSet(5, L), ref SIB);
                SetBit(3, IsBitSet(6, L), ref SIB);
                SetBit(4, IsBitSet(7, L), ref SIB);

                SetBit(5, IsBitSet(5, R), ref SIB);
                SetBit(6, IsBitSet(6, R), ref SIB);
                SetBit(7, IsBitSet(7, R), ref SIB);
                //return
                return new byte[1] { SIB };
            }


            // RETURN IF NO ONE CONTAINES REG

            if (ContainsRegister(splitplus[0], true) == -1 && ContainsRegister(splitplus[1], true) == -1)
            {
                logR.Add("NO REGISTER IN SIB ");
                return null;
            }

            // CHECK IF ALL 2 CONTAINS REG
            if (ContainsRegister(splitplus[0], true) > -1 && ContainsRegister(splitplus[1], true) > -1)
            {
                // MEANS BASE IS A REG
                string isc = "";
                if (L != 255)
                {
                    SetBit(5, IsBitSet(5, L), ref SIB);
                    SetBit(6, IsBitSet(6, L), ref SIB);
                    SetBit(7, IsBitSet(7, L), ref SIB);

                    // GET SCALE AND INDEX in splitplus[1]
                    isc = splitplus[1];
                    // RETURN
                }
                else
                {
                    SetBit(5, IsBitSet(5, R), ref SIB);
                    SetBit(6, IsBitSet(6, R), ref SIB);
                    SetBit(7, IsBitSet(7, R), ref SIB);

                    // GET SCALE AND INDEX in splitplus[0]
                    isc = splitplus[0];

                }

                string[] splitmul = isc.Split('*');
                if (splitmul.Length != 2)
                {
                    logR.Add("BAD MUL SIZE" + isc);
                    return null;
                }


                byte LL = GetReg_SIB(splitmul[0]);
                byte RR = GetReg_SIB(splitmul[1]);

                if (LL == 255 && RR == 255)
                {
                    logR.Add("NO REGISTER FOUND");
                    return null;
                }


                // buid index
                if (LL != 255)
                {

                    logR.Add("SIB INDEX: " + LL);
                    PrintByte(LL);
                    SetBit(2, IsBitSet(5, LL), ref SIB);
                    SetBit(3, IsBitSet(6, LL), ref SIB);
                    SetBit(4, IsBitSet(7, LL), ref SIB);
                }
                else
                {
                    logR.Add("SIB INDEX: " + RR);
                    PrintByte(RR);
                    SetBit(2, IsBitSet(5, RR), ref SIB);
                    SetBit(3, IsBitSet(6, RR), ref SIB);
                    SetBit(4, IsBitSet(7, RR), ref SIB);
                }

                // build scale 
                int scale = -1;

                if (!int.TryParse(splitmul[0], out scale))
                {
                    if (!int.TryParse(splitmul[1], out scale))
                    {
                        logR.Add("NO SCALE");
                        return null;
                    }
                }

                if (scale == 1 || scale == 2 || scale == 4 || scale == 8)
                {
                    switch (scale)
                    {
                        case 1:
                            SetBit(0, false, ref SIB);
                            SetBit(1, false, ref SIB);
                            break;
                        case 2:
                            SetBit(0, false, ref SIB);
                            SetBit(1, true, ref SIB);
                            break;
                        case 4:
                            SetBit(0, true, ref SIB);
                            SetBit(1, false, ref SIB);
                            break;
                        case 8:
                            SetBit(0, true, ref SIB);
                            SetBit(1, true, ref SIB);
                            break;
                    }
                }
                else
                {
                    return null;
                }

                // RETURN 

                return new byte[1] { SIB };
            }
            string[] splitmul2;

            if (ContainsRegister(splitplus[0], true) > -1)
            {
                splitmul2 = splitplus[0].Split('*');
                if (splitmul2.Length != 2)
                {
                    return null;
                }
                int disp32;
                int scale;
                byte index;
                // base is 1 0 1 
                SetBit(5, true, ref SIB);
                SetBit(6, false, ref SIB);
                SetBit(7, true, ref SIB);

                if (!int.TryParse(splitplus[1], out disp32))
                {
                    return null;
                }
                if (!int.TryParse(splitmul2[0], out scale))
                {
                    if (!int.TryParse(splitmul2[1], out scale))
                    {
                        logR.Add("NO SCALE");
                        return null;
                    }
                    else
                    {
                        index = GetReg_SIB(splitmul2[0]);
                    }

                }
                else
                {
                    index = GetReg_SIB(splitmul2[1]);
                }

                if (index == 255)
                {
                    logR.Add("NO INDEX");
                    return null;
                }
                SetBit(2, IsBitSet(5, index), ref SIB);
                SetBit(3, IsBitSet(6, index), ref SIB);
                SetBit(4, IsBitSet(7, index), ref SIB);
                if (scale == 1 || scale == 2 || scale == 4 || scale == 8)
                {
                    switch (scale)
                    {
                        case 1:
                            SetBit(0, false, ref SIB);
                            SetBit(1, false, ref SIB);
                            break;
                        case 2:
                            SetBit(0, false, ref SIB);
                            SetBit(1, true, ref SIB);
                            break;
                        case 4:
                            SetBit(0, true, ref SIB);
                            SetBit(1, false, ref SIB);
                            break;
                        case 8:
                            SetBit(0, true, ref SIB);
                            SetBit(1, true, ref SIB);
                            break;
                    }
                }
                else
                {
                    return null;
                }


                dsp = BitConverter.GetBytes(disp32);

                // RETURN
                return new byte[5] { SIB, dsp[0], dsp[1], dsp[2], dsp[3] }; // revoir l'ordre ... 

            }
            else
            {
                splitmul2 = splitplus[1].Split('*');
                if (splitmul2.Length != 2)
                {
                    return null;
                }
                int disp32;
                int scale;
                byte index;
                // base is 1 0 1 
                SetBit(5, true, ref SIB);
                SetBit(6, false, ref SIB);
                SetBit(7, true, ref SIB);

                if (!int.TryParse(splitplus[0], out disp32))
                {
                    return null;
                }
                if (!int.TryParse(splitmul2[0], out scale))
                {
                    if (!int.TryParse(splitmul2[1], out scale))
                    {
                        return null;
                    }
                    else
                    {
                        index = GetReg_SIB(splitmul2[0]);
                    }

                }
                else
                {
                    index = GetReg_SIB(splitmul2[1]);
                }

                if (index == 255)
                {
                    return null;
                }
                SetBit(2, IsBitSet(5, index), ref SIB);
                SetBit(3, IsBitSet(6, index), ref SIB);
                SetBit(4, IsBitSet(7, index), ref SIB);
                if (scale == 1 || scale == 2 || scale == 4 || scale == 8)
                {
                    switch (scale)
                    {
                        case 1:
                            SetBit(0, false, ref SIB);
                            SetBit(1, false, ref SIB);
                            break;
                        case 2:
                            SetBit(0, false, ref SIB);
                            SetBit(1, true, ref SIB);
                            break;
                        case 4:
                            SetBit(0, true, ref SIB);
                            SetBit(1, false, ref SIB);
                            break;
                        case 8:
                            SetBit(0, true, ref SIB);
                            SetBit(1, true, ref SIB);
                            break;
                    }
                }
                else
                {
                    return null;
                }
                dsp = BitConverter.GetBytes(disp32);

                return new byte[5] { SIB, dsp[0], dsp[1], dsp[2], dsp[3] }; // revoir l'ordre ... 
            }

        }

        public static int GetOpcode(int mnemoid, int opsize, bool imm, bool d, bool s)
        {
            if (opsize == 1)
            {
                for (byte i = 0; i < map.Length; i++)
                {
                    if (IsBitSet(7, i) == s && IsBitSet(5, i) == imm && map[i] == mnemoid)
                    {
                        return i;
                    }
                }
            }
            else
            {
                for (byte i = 0; i < map.Length; i++)
                {
                    if (IsBitSet(7, i) == s && IsBitSet(6, i) == d && IsBitSet(5, i) == imm && map[i] == mnemoid)
                    {
                        return i;
                    }
                }

            }

            return -1;
        }

        public static byte[] Encode(byte t1, byte t2, string opstr1, string opstr2, bool s, byte SIB, byte[] SIBDISP, int mnemoid, int spos)
        {

            // _________ case t0 ____________
            /*
                op imm
                op reg
                op reg, imm
                op reg, reg
             
             */
            byte MRR = 0;
            byte opcode = 0;
            byte[] dsp = null;
            byte[] imm = null;


            bool d = false;

            if (t1 == 0 && t2 == 255)
            {
                imm = BitConverter.GetBytes(int.Parse(opstr1));

                int r = GetOpcode(mnemoid, 1, true, d, s);
                if (r == -1)
                    return null; //err
                opcode = (byte)r;

                return new byte[] { opcode, MRR, imm[0], imm[1], imm[2], imm[3] };
            }
            else if (t1 == 1 && t2 == 255)
            {
                SetBit(0, true, ref MRR);
                SetBit(1, true, ref MRR);

                byte rgval = GetReg_REGRM(opstr1)[0];

                SetBit(5, IsBitSet(5, rgval), ref MRR);
                SetBit(6, IsBitSet(6, rgval), ref MRR);
                SetBit(7, IsBitSet(7, rgval), ref MRR);

                int r = GetOpcode(mnemoid, 1, false, d, s);
                if (r == -1)
                    return null; //err

                opcode = (byte)r;


                return new byte[] { opcode, MRR };
            }
            else if (t1 == 1 && t2 == 0)
            {
                SetBit(0, true, ref MRR);
                SetBit(1, true, ref MRR);

                byte rgval = GetReg_REGRM(opstr1)[0];
                SetBit(5, IsBitSet(5, rgval), ref MRR);
                SetBit(6, IsBitSet(6, rgval), ref MRR);
                SetBit(7, IsBitSet(7, rgval), ref MRR);

                int r = GetOpcode(mnemoid, 2, true, d, s);
                if (r == -1)
                    return null; //err
                opcode = (byte)r;

                imm = BitConverter.GetBytes(int.Parse(opstr2));

                return new byte[] { opcode, MRR, imm[0], imm[1], imm[2], imm[3] };
            }
            else if (t1 == 1 && t2 == 1)
            {
                // d is 0. so put opstr1 as rm. opstr2 as reg (d0 is reg to rm) (d1 is rm to reg)
                SetBit(0, true, ref MRR);
                SetBit(1, true, ref MRR);

                byte rgval = GetReg_REGRM(opstr1)[0];
                SetBit(5, IsBitSet(5, rgval), ref MRR);
                SetBit(6, IsBitSet(6, rgval), ref MRR);
                SetBit(7, IsBitSet(7, rgval), ref MRR);

                rgval = GetReg_REGRM(opstr2)[0];
                SetBit(2, IsBitSet(5, rgval), ref MRR);
                SetBit(3, IsBitSet(6, rgval), ref MRR);
                SetBit(4, IsBitSet(7, rgval), ref MRR);

                int r = GetOpcode(mnemoid, 2, false, d, s);
                if (r == -1)
                    return null; //err
                opcode = (byte)r;

                return new byte[] { opcode, MRR };
            }

            // Build R/M bits
            byte rmfrmt = t1;
            string strfrmt = opstr1;
            if (t2 > 1 && t2 != 255)
            {
                rmfrmt = t2;
                strfrmt = opstr2;
                // set dir 
                d = true;
            }


            // find the correspondant opcode by mnemonic. 
            if (t2 == 255)
            {
                int r = GetOpcode(mnemoid, 1, false, d, s);
                if (r == -1)
                    return null;
                opcode = (byte)r;
            }
            else
            {
                int r = GetOpcode(mnemoid, 2, false, d, s);
                if (r == -1)
                    return null;
                opcode = (byte)r;
            }

            //
            // d is set
            /*
        case 
       2:[reg]
       3:[disp32]
       5:[reg-disp8]
       4:[reg-disp32]
       8:[reg+disp8]
       7:[reg+disp32]
       6:[reg+reg]
       6:[reg+reg*scale]
       6:[disp+reg*scale]
       6:[disp+reg]
        */


            // get the 

            // MOD && R/M 
            switch (rmfrmt)
            {
                case 2: // [reg]        -> mod: 0 0 -> r/m : reg-byte
                    SetBit(0, false, ref MRR);
                    SetBit(1, false, ref MRR);

                    byte rgval = GetReg_REGRM(strfrmt.Substring(1, strfrmt.Length - 2))[0];
                    logR.Add("RM IS " + rgval);
                    if (IsBitSet(7, rgval))
                    {
                        logR.Add("set...");
                    }
                    SetBit(5, IsBitSet(5, rgval), ref MRR);
                    SetBit(6, IsBitSet(6, rgval), ref MRR);
                    SetBit(7, IsBitSet(7, rgval), ref MRR);

                    break;
                case 3: // [disp32]     -> mod: 0 0 -> r/m : 1 0 1 -> disp32

                    SetBit(0, false, ref MRR);
                    SetBit(1, false, ref MRR);

                    SetBit(5, true, ref MRR);
                    SetBit(6, false, ref MRR);
                    SetBit(7, true, ref MRR);

                    int dsp32 = int.Parse(strfrmt.Substring(1, strfrmt.Length - 2));
                    dsp = BitConverter.GetBytes(dsp32);

                    break;
                case 5: // [reg-disp8]  -> mod: 0 1 -> r/m : reg-byte -> -disp(8)
                    SetBit(0, false, ref MRR);
                    SetBit(1, true, ref MRR);

                    string strp = strfrmt.Substring(1, strfrmt.Length - 2);
                    // detect side
                    string[] parser = strp.Split('-');
                    int dsp8 = 0;
                    int regside = 0;
                    if (!int.TryParse(parser[1], out dsp8))
                    {
                        int.TryParse(parser[0], out dsp8);
                        regside = 1;
                    }
                    rgval = GetReg_REGRM(parser[regside])[0];
                    SetBit(5, IsBitSet(5, rgval), ref MRR);
                    SetBit(6, IsBitSet(6, rgval), ref MRR);
                    SetBit(7, IsBitSet(7, rgval), ref MRR);

                    dsp = new byte[1] { (byte)-dsp8 };

                    break;
                case 4: // [reg-disp32] -> mod: 1 0 -> r/m : reg-byte -> -disp(32)
                    SetBit(0, true, ref MRR);
                    SetBit(1, false, ref MRR);

                    strp = strfrmt.Substring(1, strfrmt.Length - 2);
                    // detect side
                    parser = strp.Split('-');
                    dsp32 = 0;
                    regside = 0;
                    if (!int.TryParse(parser[1], out dsp32))
                    {
                        int.TryParse(parser[0], out dsp32);
                        regside = 1;
                    }
                    rgval = GetReg_REGRM(parser[regside])[0];
                    SetBit(5, IsBitSet(5, rgval), ref MRR);
                    SetBit(6, IsBitSet(6, rgval), ref MRR);
                    SetBit(7, IsBitSet(7, rgval), ref MRR);

                    dsp = BitConverter.GetBytes(-dsp32);

                    break;
                case 8: // [reg+disp8]  -> mod: 0 1 -> r/m : reg-byte -> disp(8)
                    SetBit(0, false, ref MRR);
                    SetBit(1, true, ref MRR);
                    strp = strfrmt.Substring(1, strfrmt.Length - 2);
                    // detect side
                    parser = strp.Split('+');
                    dsp8 = 0;
                    regside = 0;
                    if (!int.TryParse(parser[1], out dsp8))
                    {
                        int.TryParse(parser[0], out dsp8);
                        regside = 1;
                    }
                    rgval = GetReg_REGRM(parser[regside])[0];
                    SetBit(5, IsBitSet(5, rgval), ref MRR);
                    SetBit(6, IsBitSet(6, rgval), ref MRR);
                    SetBit(7, IsBitSet(7, rgval), ref MRR);

                    dsp = new byte[1] { (byte)dsp8 };

                    break;
                case 7: // [reg+disp32] -> mod: 1 0 -> r/m : reg-byte -> disp(32)
                    SetBit(0, true, ref MRR);
                    SetBit(1, false, ref MRR);

                    strp = strfrmt.Substring(1, strfrmt.Length - 2);
                    // detect side
                    parser = strp.Split('+');
                    dsp32 = 0;
                    regside = 0;
                    if (!int.TryParse(parser[1], out dsp32))
                    {
                        int.TryParse(parser[0], out dsp32);
                        regside = 1;
                    }
                    rgval = GetReg_REGRM(parser[regside])[0];
                    SetBit(5, IsBitSet(5, rgval), ref MRR);
                    SetBit(6, IsBitSet(6, rgval), ref MRR);
                    SetBit(7, IsBitSet(7, rgval), ref MRR);

                    dsp = BitConverter.GetBytes(dsp32);

                    break;
                case 6: // SIB          -> mod: 0 0 -> r/m 1 0 0 -> sib -> sibdisp
                    SetBit(0, false, ref MRR);
                    SetBit(1, false, ref MRR);

                    SetBit(5, true, ref MRR);
                    SetBit(6, false, ref MRR);
                    SetBit(7, false, ref MRR);

                    //sibdisp if not NULL !! 

                    break;
            }

            // BUILD REG IF 2 OPERANDS ( always a reg ) 
            if (t2 != 255 && t1 != 255)
            {
                if (rmfrmt == t2 && t1 != 0)
                {
                    //build from opstr1 
                    byte rgval = GetReg_REGRM(opstr1)[0];
                    logR.Add("REG IS " + rgval);
                    SetBit(2, IsBitSet(5, rgval), ref MRR);
                    SetBit(3, IsBitSet(6, rgval), ref MRR);
                    SetBit(4, IsBitSet(7, rgval), ref MRR);
                }
                else if (t2 != 0)
                {
                    //build from opstr2 
                    byte rgval = GetReg_REGRM(opstr2)[0];
                    logR.Add("REG IS " + rgval);
                    SetBit(2, IsBitSet(5, rgval), ref MRR);
                    SetBit(3, IsBitSet(6, rgval), ref MRR);
                    SetBit(4, IsBitSet(7, rgval), ref MRR);
                }
            }

            //MessageBox.Show("0x"+MRR.ToString("X"));

            // case : no SIB | no DISP
            // case : SIB    | no DISP
            // case : no SIB |    DISP
            // case : SIB    |    DISP

            // this is big shit 
            if (SIB == 255 && dsp == null)
            {
                return new byte[] { opcode, MRR };
            }
            if (SIB != 255 && dsp == null)
            {
                return new byte[] { opcode, MRR, SIB };
            }
            if (SIB == 255 && dsp != null)
            {
                if (dsp.Length == 1)
                {
                    return new byte[] { opcode, MRR, dsp[0] };
                }
                else
                {
                    return new byte[] { opcode, MRR, dsp[0], dsp[1], dsp[2], dsp[3] };
                }

            }
            if (SIB != 255 && dsp != null)
            {

                if (dsp.Length == 1)
                {
                    return new byte[] { opcode, MRR, SIB, dsp[0] };
                }
                else
                {
                    return new byte[] { opcode, MRR, SIB, dsp[0], dsp[1], dsp[2], dsp[3] };
                }
            }

            return null; // why i need it ????


        }

        public static int ContainsRegister(string s, bool b32only)
        {
            if (s.Contains("eax")) { return 0; }
            if (s.Contains("ax") && !b32only) { return 1; }
            if (s.Contains("al")) { return 2; }
            if (s.Contains("ah")) { return 3; }

            if (s.Contains("ebx")) { return 4; }
            if (s.Contains("bx") && !b32only) { return 5; }
            if (s.Contains("bl")) { return 6; }
            if (s.Contains("bh")) { return 7; }

            if (s.Contains("edx")) { return 8; }
            if (s.Contains("dx") && !b32only) { return 9; }
            if (s.Contains("dl")) { return 10; }
            if (s.Contains("dh")) { return 11; }

            if (s.Contains("ecx")) { return 12; }
            if (s.Contains("cx") && !b32only) { return 13; }
            if (s.Contains("cl")) { return 14; }
            if (s.Contains("ch")) { return 15; }

            if (s.Contains("esp")) { return 12; }
            if (s.Contains("sp") && !b32only) { return 13; }
            if (s.Contains("ebp")) { return 14; }
            if (s.Contains("bp") && !b32only) { return 15; }
            if (s.Contains("si") && !b32only) { return 16; }
            if (s.Contains("di") && !b32only) { return 17; }
            if (s.Contains("esi")) { return 18; }
            if (s.Contains("edi")) { return 19; }


            return -1;
        }

        public static byte GetReg_SIB(string s)
        {
            switch (s.ToLower())
            {
                case "eax": return 0;
                case "ecx": return 1;
                case "edx": return 2;
                case "ebx": return 3;
                case "ebp": return 5;
                case "esi": return 6;
                case "edi": return 7;
            }
            return 255;
        }

        public static byte[] GetReg_REGRM(string s)
        {
            switch (s.ToLower())
            {
                case "al": return new byte[2] { 0, 8 };
                case "ax": return new byte[2] { 0, 16 };
                case "eax": return new byte[2] { 0, 32 };

                case "cl": return new byte[2] { 1, 8 };
                case "cx": return new byte[2] { 1, 16 };
                case "ecx": return new byte[2] { 1, 32 };

                case "dl": return new byte[2] { 2, 8 };
                case "dx": return new byte[2] { 2, 16 };
                case "edx": return new byte[2] { 2, 32 };

                case "bl": return new byte[2] { 3, 8 };
                case "bx": return new byte[2] { 3, 16 };
                case "ebx": return new byte[2] { 3, 32 };

                case "ah": return new byte[2] { 4, 8 };
                case "sp": return new byte[2] { 4, 16 };
                case "esp": return new byte[2] { 4, 32 };

                case "ch": return new byte[2] { 5, 8 };
                case "bp": return new byte[2] { 5, 16 };
                case "ebp": return new byte[2] { 5, 32 };

                case "dh": return new byte[2] { 6, 8 };
                case "si": return new byte[2] { 6, 16 };
                case "esi": return new byte[2] { 6, 32 };

                case "bh": return new byte[2] { 7, 8 };
                case "di": return new byte[2] { 7, 16 };
                case "edi": return new byte[2] { 7, 32 };
            }
            return null;
        }

        public static string GetByteString(byte b)
        {
            string s = "";
            for (int i = 0; i < 8; i++)
            {
                if (IsBitSet(i, b))
                {

                    s += "1";
                }
                else
                {
                    s += "0";
                }

            }
            return s;
        }
        public static void PrintByte(byte b)
        {
            string s = "";
            for (int i = 0; i < 8; i++)
            {
                if (IsBitSet(i, b))
                {

                    s += "1";
                }
                else
                {
                    s += "0";
                }

            }
            logR.Add(s);
        }


        public static bool IsBitSet(int p, byte b)
        {
            return (b & (1 << (7 - p))) != 0;
        }
        public static void SetBit(int p, bool val, ref byte b)
        {
            byte mask = (byte)(1 << (7 - p));
            if (val)
                b |= mask;
            else
                b &= (byte)~mask;

        }
        public static void AddBytesToList(ref List<byte> A, byte[] B)
        {
            foreach (byte b in B)
            {
                A.Add(b);
            }
        }

        public static byte[] ListToByteArray(List<byte> A)
        {
            if (A.Count == 0) { return null; }
            byte[] result = new byte[A.Count];
            for (int i = 0; i < A.Count; i++)
            {
                result[i] = A[i];
            }
            return result;
        }
    }
}
