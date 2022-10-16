import chisel3._
import chisel3.util._
import chisel3.tester._
import chisel3.tester.RawTester.test
import scala.collection._
import scala.math.log10

// module shiftReg #(
//     parameter IN_WIDTH = 123,
//     parameter OUT_WIDTH = 128,
//     parameter D_WIDTH = 128,
//     parameter INTERRUPT = {128{1'b1}}
class ShiftReg(IN_WIDTH: Int = 123, OUT_WIDTH: Int = 128, D_WIDTH: Int = 128, INTERRUPT: Int = 1<<128 - 1) extends Module {
//     input clk,
//     input rst, 
//     input en, // when high shifts signal, mask low if not ready
//     input D,
//     output mode, // 0: shifting in, 1: shifting out
//     output Q,
//     output RESET
    val io = IO(new Bundle {
        val en = Input(Bool())
        val D = Input(UInt(1.W))
        val mode = Output(Bool())
        val Q = Output(UInt(1.W))
        val RESET = Output(UInt(1.W))
    })
    
//     localparam LOG_WIDTH = $clog2(D_WIDTH) + 1;
    val LOG_WIDTH = (log10(D_WIDTH)/log10(2.0)).toInt
    
//     reg [D_WIDTH-1:0] data;
//     wire [LOG_WIDTH-1:0] counter_Q;
//     wire mode_Q;
    val data = RegInit(0.U(D_WIDTH.W))
    val counter_Q = RegInit(0.U(LOG_WIDTH.W))
    val mode_Q = RegInit(0.B)
    
//     assign Q = data[counter_Q];
//     assign mode = mode_Q;
//     // Don't reset in the middle of recieving, or it may trigger random resets.
//     assign RESET = (INTERRUPT == data) && mode_Q;
    io.Q := data(counter_Q)
    io.mode := mode_Q
    io.RESET := (INTERRUPT.U === data) && mode_Q
    
//     ila_1(  .clk(clk), .probe0(data), .probe1(counter_Q), .probe2(mode),
//             .probe3(RESET), .probe4(en), .probe5(D), .probe6(Q));
    
//     wire modeFlip;
//     assign modeFlip = (mode_Q ? (counter_Q == OUT_WIDTH-1) : (counter_Q == IN_WIDTH-1)) && en;
    val modeFlip = Wire(Bool())
    when (mode_Q) {
        modeFlip := counter_Q === (OUT_WIDTH.U - 1.U)
    }
    .otherwise {
        modeFlip := (counter_Q === (IN_WIDTH.U - 1.U)) && io.en
    }

//     RegInit #(.WIDTH(1), .INIT(0)) 
//     mode(
//         .clk(clk),
//         .rst(rst),
//         .en(en || modeFlip),
//         .D(modeFlip ? !mode : mode),
//         .Q(mode_Q)
//     );
    when (io.en || modeFlip) {
        when (modeFlip) {
            mode_Q := !mode_Q
        }
    }
        
//     RegInit #(.WIDTH(LOG_WIDTH), .INIT(0)) 
//     counter(
//         .clk(clk),
//         .rst(rst || modeFlip),
//         .en(en),
//         .D(counter_Q + 1'b1),
//         .Q(counter_Q)
//     );
    when (modeFlip) {
        counter_Q := 0.U
    }
    .elsewhen (io.en) {
        counter_Q := counter_Q + 1.U
    }

//     always @(posedge clk) begin
//         data[counter_Q] <= rst ? 1'b0 : ((en & !mode) ? D : data[counter_Q]);
//     end
    when (io.en & !mode_Q) {
        data(counter_Q) := io.D
    }
}
