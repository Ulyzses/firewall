<script lang="ts">
  import { getModalStore } from '@skeletonlabs/skeleton';
  import type { ModalSettings, ModalComponent, ModalStore } from '@skeletonlabs/skeleton';
  import plug from '$lib/assets/plug.svg';
  import addDeviceModalComponent from '$lib/components/Modal.svelte';
  import { SlideToggle } from '@skeletonlabs/skeleton';

  const modalComponent: ModalComponent = { ref: addDeviceModalComponent };
  const modalStore = getModalStore();

  type formData = {
		name: string;
		macList: string[];
	};

  const modal: ModalSettings = {
    type: 'component',
    component: modalComponent,
    backdropClasses: '!backdrop-blur-md',
    modalClasses: '!bg-white',
    title: 'Add Device',
    response(r : formData | undefined | boolean) {
      if (typeof(r) == 'object'){
        addDeviceMac = r.macList.join(':');
        addDeviceName = r.name;
        addDevice();
      }
      console.log(r);
    },
  };
  
  let addDeviceMac = '';
  let addDeviceName= '';

  const statusColors = ['text-firewall-red', 'text-green-500'];
  const statusLabels = ['OFF', 'ON'];
              
  interface Device {
    name: string;
    mac: string;
    status: boolean;
    smoke: boolean;
  }

  export let data;

  $: ({ user, supabase, devices } = data);

  async function addDevice() {
    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: addDeviceMac,
        name: addDeviceName
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }

  async function toggleDevice(device: Device) {
    const { data, error } = await supabase
      .from('actions')
      .insert({
        mac: device.mac,
        action: device.status ? 'OFF' : 'ON'
      })
      .select();
    
    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }

    devices = devices;
  }
</script>

<div class='grid grid-flow-row grid-cols-1 lg:grid-cols-2 gap-4 mx-6 overflow-y-auto'>
  <div class='flex flex-row justify-between w-full h-min lg:col-span-2 items-center'>
    <h1 class='text-xl sm:text-3xl font-kollektif'>Devices</h1>
    <button class='border border-white bg-firewall-red p-2 sm:p-4 rounded-lg w-32 sm:w-40 h-min text-sm text-white font-DM hover:border-firewall-red' on:click={() => { modalStore.trigger(modal); }}>+ Add Device</button>
  </div>
  {#if devices.some((x) => x.smoke)}
    <div class='flex flex-row space-x-2 justify-left items-center text-xs font-bold text-lg py-2 s-auto rounded-lg lg:col-span-2'>
      <span class='shrink-0 bg-firewall-red text-white text-lg w-8 h-8 rounded-full flex justify-center items-center'>!</span>
      <span class='text-firewall-red sm:text-base'>WARNING: One of your devices has detected smoke and has been turned off automatically.</span>
    </div>
  {/if}
  {#each devices as device (device.id)}
    <div class='flex flex-row flex-none w-auto h-min pl-4 pr-2 py-4 sm:p-4 sm:p-8 text-sm sm:text-base items-center bg-gray-200 rounded-lg border border-gray-200 font-DM gap-6 justify-between font-bold'>
      <img src={ plug } alt='' class='size-20 sm:size-24 md:size-32' />
      <div class='basis-1/3 h-auto w-1/4 grid grid-flow-cols justify-items-left justify-self-center'>
        <span class='text-base sm:text-lg w-full truncate'>{ device.name }</span>
        <span class='italic w-full truncate'>{ device.mac }</span>
          <span class='text-firewall-red text-xs sm:text-sm w-full truncate'>   
          {#if device.smoke}
            Smoke Detected!
          {:else}
            <br>
          {/if}
        </span>
      </div>
      <div class='flex flex-col h-full w-1/4 text-center justify-center items-center md:text-lg'>
        <span class='font-bold { statusColors[Number(device.status)] }'>{ statusLabels[Number(device.status)] }</span>
        <SlideToggle name="slide" bind:checked={device.status} active="bg-green-500" background='bg-firewall-red' size="md" on:click={() => toggleDevice(device)} />
        </div>
    </div>
  {/each}
</div>